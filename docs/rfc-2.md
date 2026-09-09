# RFC-2: Stack Structure

Status: draft

> Superseded by RFC-11 (Building Blocks). Kept for history.

## Stack Frame

Stack Frame consists of two parts:
 * regular part
 * irregular part

A stack frame may have the following utility functions associated with it:
 * `init_fn` prepares the regular part for the usage (e.g. construct predefined values, allocates resources)
 * `deinit_fn` releases resources associated with the stack frame
 * `post_realloc_fn` postprocessing for the stack frame after it was reallocated to a new address (needed for stack memory reallocation)

These functions likely manipulate only with the regular part of the stack frame.

**Resolved (GH-1)**: stack frame values are expected to be trivially
relocatable -- movable to a new address via a plain byte copy, with no
per-value fixup needed. This is the same property proposed for the C++
standard library as "trivially relocatable" (WG21 P1144). Many
standard-library types, such as `std::vector`, `std::unique_ptr`, and
`std::shared_ptr`, are trivially relocatable in practice because they
contain no self-references. This suggests that the requirement is not
overly restrictive. `std::string` is a notable counterexample because of
the small-string optimization.

For a value that genuinely isn't trivially relocatable, the pattern is to
store it on the heap and keep only a pointer to it on the stack, so the
stack's own move never has to touch it.


## Stack reallocation

Stack is represented by a continuous memory region, a part of which can be currently unused.

The initial allocation is a configuration choice, not a maximum stack size.
Pushing a frame grows the allocation geometrically when the existing capacity
cannot hold the frame header, fixed locals, and the frame's declared extra
capacity. Growth may move the complete stack allocation. Persistent references
to stack data must therefore be offsets; pointers returned by stack accessors
remain valid only until an operation that can push a frame or grow the stack.

A failed growth leaves the old allocation, capacity, offsets, frames, and
frame contents unchanged.

The total amount of memory allocated for that stack is _capacity_. _Top_ points to the first _unused_ address in the stack. When we need to push something to the stack, we write it to the _top_ location and modify _top_ to point after the newly written value.

The memory in the stack is segmented into stack frames. Each stack frame has a fixed part and an irregular part. The fixed part gets created when a new stack frame is pushed to the stack, and destroyed when the frame is poped from the stack. The irregular part of the top-most stack frame may change -- new values can be pushed/popped from it. Each stack frame has an associated stack frame layout that, among other things, specifies the maximal size of the frame (the
regular part has a fixed size, and there is a bound on the irregular part size). The idea is that pushing a new frame to the stack can trigger a memory allocation to get more capacity for the new frame, but manipulations with the
irregular stack's part must involve no extra memory allocations/checks for the stack.

## Global statics

The first frame in the stack declares the global variables.
