# RFC-11: Building Blocks

Status: draft; everything described here is built and in use.

## Overview

Four pieces compose the runtime:

```
allocator -> stack -> frame (+ frame layout) -> state (dispatch loop)
```

- **allocator** (`public/allocator.h`) supplies raw memory; `stack`/`state`
  never call `malloc` directly.
- **stack** (`public/stack.h`) is one contiguous, growable region holding
  every live frame plus the top frame's temporaries.
- **frame / frame layout** describes one activation: header, fixed-size
  locals, declared temporary/extra capacity, and lifecycle callbacks.
- **state** (`public/state.h`) is the dispatch loop: instruction pointer,
  instruction table, and the one stack.


## Allocator

```c
struct u7_vm_allocator {
  void* data;
  allocate_fn; reallocate_fn; deallocate_fn;
};
```

The `struct u7_vm_allocator` is a simple allocator interface. The `data` field
points to implementation-specific state that is passed to each allocator
operation.

The interface does not imply ownership of the allocator implementation. The
owner of the allocator state must ensure that it remains valid for as long as
the interface is in use.

The `allocate_fn`, `reallocate_fn`, and `deallocate_fn` callbacks follow the
semantics of `malloc`, `realloc`, and `free`, respectively, with an additional
`data` argument providing access to the implementation-specific state.

Memory allocated through an allocator must be deallocated through the same
allocator.

`u7_vm_limited_allocator` wraps an upstream allocator and enforces a logical
byte budget (`used`/`limit`). In particular, it may return `NULL` when the
budget is exhausted even though the operating system still has memory
available.

`u7_error` continues to use plain libc `malloc`/`free`.

First, many general-purpose operating systems overcommit virtual memory by
default. Linux's heuristic overcommit mode is a common example: actual physical
memory exhaustion may cause the kernel to kill a process rather than fail the
allocation that triggered it. A more reliable way to enforce a memory budget is
therefore to set an application-level limit below the system's capacity and
report an error when that limit is reached.

Consequently, reaching the application-level limit does not necessarily mean
that the system itself is out of memory; additional memory may still be
available. Keeping `u7_error` outside that budget allows this remaining memory
to be used to report the failure. Since `u7_error` is expected to consume little
memory, this should not materially affect the memory limit.

Second, the primary purpose of `u7_error` is to describe a failure after it has
occurred. If allocating the error itself fails, that introduces a second failure
while the original one is already being handled. Such a situation is difficult
to recover from and may reasonably be treated as unrecoverable. C++ follows a
similar principle in some failure paths: if a function invoked during stack
unwinding exits by throwing another exception, `std::terminate` is called.

For these reasons, `u7_error` remains outside the VM's memory budget and uses
the system allocator directly.


## Stack

The VM stack is stored in a single contiguous allocation and is logically
divided into frames:

```text
[frame 0: header | globals | extra][frame 1: header | locals | extra] ... top
```

In the current context, `base_offset` points to the header of the current (top)
frame, while `top_offset` points to the first free byte.

The stack always starts with a frame at `base_offset = 0`. The locals of this
initial frame are referred to as globals; for example, they may reserve space
for describing exceptional state. Because this frame has negligible overhead and
simplifies the implementation, the VM requires it to always be present.

### Relocation

The contiguous stack allocation is growable. Increasing its capacity may
therefore move the entire allocation.

Consequently, every value stored inline anywhere on the VM stack must be
trivially relocatable. This includes frame headers, globals, locals, and values
stored in a frame's extra space. The complete stack may be moved with a plain
`memcpy`, with no type-specific relocation or fixups, and every value must
remain valid afterward.

This requirement also determines how references involving stack memory are
represented. References to memory outside the stack may be ordinary pointers.
References to memory within the stack must instead be represented as offsets and
resolved when used.

Values that require a stable address must likewise be allocated outside the
stack, with only a reference stored inline.

We do not expect this requirement to be especially limiting in practice. Many
common value representations consist of inline data together with pointers to
external storage and therefore require no address-dependent fixup when moved.
Typical implementations of C++ types such as `std::vector` and `std::unique_ptr`
have this shape, as do ordinary plain aggregates.

The important counterexample is a representation containing a pointer into
itself. A short-string-optimized `std::string`, for example, may contain a
pointer to its own inline buffer. Copying its bytes to another address could
leave that pointer referring to the old location.

### Frame representation

Each frame begins with a header containing the offset of the previous frame, a
reference to the `u7_vm_stack_frame_layout` describing the frame, and a
`return_ip`.

Following the representation rules above, the frame header is itself trivially
relocatable. The previous frame is referenced by an offset, while the `layout`
pointer refers to data outside the stack. The meaning of `return_ip` is
described under frame lifecycle below.

Following the header, each frame contains its locals and additional reserved
stack space. Its `u7_vm_stack_frame_layout` specifies:

* `locals_size`, which controls how much space is automatically allocated for
  locals after the frame header;
* `extra_capacity`, which controls how much additional stack space is reserved
  after the locals;
* `init_fn`, which runs once the frame has been pushed onto the stack. One
  expected responsibility is initializing the locals, though this is not
  required;
* `deinit_fn`, which runs when the frame is popped. Symmetrically, one expected
  responsibility is deinitializing the locals; and
* `exception_handler_fn`, which is invoked when the current exception status
  falls within the scope of the frame. It is responsible either for recovering
  from the exception and updating `state->ip` to the next instruction to
  execute, or for transitioning to `UNWIND`.

If code running in a frame has a known upper bound on the additional stack space
it may require, `extra_capacity` allows that space to be reserved when the frame
is pushed. Individual instructions then need not perform their own
stack-capacity checks.

### Frame lifecycle

`u7_vm_stack_push_frame(self, frame_layout, return_ip)` reserves space for the
frame header, locals, and `extra_capacity`, stores the frame metadata, runs
`init_fn`, and makes the new frame current.

`return_ip` is stored verbatim in the frame header. During ordinary frame
manipulation, the base VM deliberately assigns no precise control-flow semantics
to it.

`u7_vm_stack_pop_frame(self)` runs the current frame's `deinit_fn`, restores the
stack to the previous frame, and returns the popped frame's `return_ip`.

The intended role of `return_ip` is to identify an execution position in the
previous frame's code associated with leaving the current frame. It must always
refer to a valid instruction, but its exact meaning is intentionally left
unspecified. For example, after a function call, the caller might resume at
`return_ip`, at `return_ip + 1`, or might ignore the value entirely. The client
of the stack API is responsible for supplying return_ip when pushing a frame
and interpreting the value returned when the frame is popped, as required by
the instruction set they implement.

This is because function calls are not a base-VM concept. At this level,
`return_ip` is not necessarily the instruction following a call, and popping a
frame does not itself imply any particular control-flow operation.

However, exception propagation is the one case in which the base VM assigns
`return_ip` a fixed meaning. When a frame is popped while unwinding an exception,
the base VM unconditionally assigns that frame's `return_ip` to `state->ip`.


### Alternative to trivial relocatability

In an alternative design, we could make `extra_capacity` a hard per-frame limit
and relax the requirement that the entire stack occupy a single growable
allocation. Existing frames would then not need to move when additional stack
storage is required, making it possible to support inline values that are not
trivially relocatable.

We believe that this design would add some memory and performance overhead. We
do not currently have enough evidence that this tradeoff would improve on the
present design, so the simpler single-allocation representation is retained for
now.


## Unwinding

Each frame layout may specify an `exception_handler_fn`. When an instruction
raises an exception, `u7_vm_state_handle_exception` invokes the handler to
determine the next action: the handler either returns `RECOVER`, keeping the
frame and resuming execution at a specified `ip`, or tells the VM to `UNWIND`,
causing it to run `deinit_fn`, pop the frame, and continue unwinding in the
previous frame.

Unwinding stops, unhandled, at the root frame (`base_offset == 0`). The root
frame is never popped, so an unhandled exception leaves the globals readable
rather than tearing down the entire VM stack.

There is no dedicated cleanup mechanism for values on the stack. The exception
handler may be responsible for performing the cleanup. Alternatively, the
program may contain explicit cleanup code, with the exception handler
responsible for setting `ip` to the appropriate cleanup sequence.
