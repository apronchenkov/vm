# RFC-2: Stack Structure

Status: draft

## Stack Frame

Stack Frame consists of two parts:
 * regular part
 * irregular part

A stack frame may have the following utility functions associated with it:
 * `init_fn` prepares the regular part for the usage (e.g. construct predefined values, allocates resources)
 * `deinit_fn` releases resources associated with the stack frame
 * `post_memcpy_fn` postprocessing for the stack frame's memory region after it was `memmove`'d to a new address (needed for stack memory reallocation)

These functions likely manipulate only with the regular part of the stack frame.

TODO(apronchenkov): Find a way to avoid moving data during the stack capacity allocations. The primary issue is that it's technically hard to handle the irregular values during the reallocation. Although, such a change may not help with
deinitialization for the irregular values.


## Stack reallocation

Stack is represented by a continuous memory region, a part of which can be currently unused.

The total amount of memory allocated for that stack is _capacity_. _Top_ points to the first _unused_ address in the stack. When we need to push something to the stack, we write it to the _top_ location and modify _top_ to point after the newly written value.

The memory in the stack is segmented into stack frames. Each stack frame has a fixed part and an irregular part. The fixed part gets created when a new stack frame is pushed to the stack, and destroyed when the frame is poped from the stack. The irregular part of the top-most stack frame may change -- new values can be pushed/popped from it. Each stack frame has an associated stack frame layout that, among other things, specifies the maximal size of the frame (the
regular part has a fixed size, and there is a bound on the irregular part size). The idea is that pushing a new frame to the stack can trigger a memory allocation to get more capacity for the new frame, but manipulations with the
irregular stack's part must involve no extra memory allocations/checks for the stack.

## Global statics

The first frame in the stack declares the global variables.
