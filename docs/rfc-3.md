# RFC-3: Memory Allocator

Status: draft

> Superseded by RFC-11 (Building Blocks), which builds on the allocator
> ownership/multiplicity decision converged on below. Kept for history.

## Current decision

A `u7_vm_allocator` accounts only for memory owned directly by a subsystem;
it deliberately excludes error payloads, libc bookkeeping, and
operating-system overhead.

The allocator is designed to be threaded through VM state and stack
initialization by value, not held as a process-global singleton, so each
state can carry its own budget. Its data must outlive the state and every
allocation made through that interface.

The allocator is exposed as a small interface value (see
`public/allocator.h`) rather than a concrete type, so a deterministic test
allocator and a production limited allocator that tracks a logical memory
budget can sit behind the same shape without the caller knowing which one
it's holding.

We cannot reliably handle an out-of-memory condition when the operating
system itself runs out of memory, since even error handling may require
additional allocations -- we therefore ultimately have to assume the system
allocator (e.g. `malloc`) remains available. For normal work, however, a
custom allocator with its own logical memory budget lets us reject an
allocation before the system is actually exhausted; normal work paths should
respect that limit and handle reaching it reliably.

## Original proposal (historical)

Memory Allocator is a fundamental entity, and it seems reasonable to provide a customisation point for the user here.

There are two open questions:

1. How many allocators can coexist in a process (**resolved above**: "multiple," one per VM state, passed by value, not a process-global singleton):

  * a global singleton -- the main drawback is that the configuration of the allocator must happen globally, per process.
  * global memory allocator per subsystem
  * "multiple" -- in that case, there are two follow-up questions:
	+ how to pass an allocator to a library?
	+ what is a mechanism/contract to control the lifetime of an allocator.

2. Do we use malloc/realloc/free from the C standard library in the u7_vm project (and as an implication, in u7_error)?


Side notes:
 * There is a dependency loop between Error and MemoryAllocator interfaces.\
   (a) A memory allocation by its nature can return an error, i.e. if there is not enough memory left.\
   (b) At the same time, an error often holds some additional context describing the error state that requires a memory allocation. Like an error message.\
   It is worth noting that (b) doesn't mean that the Error interface has to be aware of MemoryAllocator.\
   In particular, it's beneficial if an error instance can be constructed without extra memory allocations.\
   The implication is more subtle that some form of a contract needs to be possible. If an error instance uses a memory allocator, the memory allocator outlives the error instance (the error instance needs to deallocate its state at some point).




## Exploring options for "multiple" allocators per process

1. External guarantees of the allocators' lifetime

1.1. u7_error uses a globally assigned memory allocator

De facto, it's what the current implementation does. `u7_vm` uses a locally assigned memory allocator, and `u7_error` uses malloc/free from the c standard library.

Pros:
 * easy to comprehend, "obviously correct."
Cons:
 * no local customisation per system instance, e.g., all `u7_error` instances use the same memory allocator.

1.2. u7_error uses a memory allocator associated with a subsystem that raised an error

Can we make the user guarantee the lifetime?

Imagine that we have subsystems `A` and `B` with different memory allocators. `fn()` uses both `A` and `B` that can return errors: `u7_error err = fn(A, B)`. Here `err` must know which allocator was used for the data, so `u7_error_release(err)` could properly release the resources.

<work-in-progress>
