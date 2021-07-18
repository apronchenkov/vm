# RFC-2: Memory Allocator

Status: draft

Memory Allocator is a fundamental entity, and it seems reasonable to provide a customisation point for the user here.

There are two open questions:

1. How many allocators can coexist in a process:

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
