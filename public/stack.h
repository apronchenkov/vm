#ifndef U7_VM_STACK_H_
#define U7_VM_STACK_H_

#include "@/public/allocator.h"
#include "@/public/memory_utils.h"

#include <github.com/apronchenkov/u7_init/public/init.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_state;

// Procedures for a stack frame initialization and deconstruction.
struct u7_vm_stack_frame_layout;

// Initializes state for a stack frame.
typedef void (*u7_vm_stack_frame_layout_init_fn_t)(
    struct u7_vm_stack_frame_layout const* self, void* memory);

// Deinitializes state of a stack frame.
typedef void (*u7_vm_stack_frame_layout_deinit_fn_t)(
    struct u7_vm_stack_frame_layout const* self, void* memory);

// Result of consulting a frame's exception handler.
enum u7_vm_exception_handler_action {
  // Unwind this frame and continue searching in its caller.
  U7_VM_EXCEPTION_HANDLER_ACTION_UNWIND,
  // Keep this frame; the handler has set `state->ip` to the instruction
  // where execution should resume.
  U7_VM_EXCEPTION_HANDLER_ACTION_RECOVER,
};

// Handles an exception for the current stack frame.
//
// Called while `state->status` is U7_VM_STATE_STATUS_EXCEPTION.
// `state->ip` identifies the instruction associated with the exception in the
// current frame: the instruction that raised it in the originating frame, or
// the instruction at which the frame was suspended after unwinding a callee.
typedef enum u7_vm_exception_handler_action (*u7_vm_exception_handler_fn_t)(
    struct u7_vm_state* state, void* data);

// A stack frame layout.
//
// NOTE: All values stored on the stack, including this frame's locals, must be
// trivially relocatable as defined by RFC-2 GH-1. Growing the stack may move
// them with a plain byte copy and performs no per-frame fixup.
struct u7_vm_stack_frame_layout {
  size_t locals_size;
  size_t extra_capacity;
  u7_vm_stack_frame_layout_init_fn_t /*nullable*/ init_fn;
  u7_vm_stack_frame_layout_deinit_fn_t /*nullable*/ deinit_fn;
  u7_vm_exception_handler_fn_t /*nullable*/ exception_handler_fn;
  void* exception_handler_data;
  const char* description;
};

// A stack header of a stack frame.
struct u7_vm_stack_frame_header {
  size_t old_base_offset;
  struct u7_vm_stack_frame_layout const* frame_layout;
  size_t return_ip;
};

enum {
  U7_VM_STACK_FRAME_HEADER_SIZE =
      (sizeof(struct u7_vm_stack_frame_header) + U7_VM_DEFAULT_ALIGNMENT - 1) &
      -(size_t)U7_VM_DEFAULT_ALIGNMENT
};

//   ...
// <frame start>
//   return_instruction_address
//   optional{return_value_address}
// base:
//   stack_frame_header
//   ...
// top:
// <frame end>

struct u7_vm_stack {
  void* memory;
  size_t base_offset;  // offset to the frame base
  size_t top_offset;   // offset to the stack top
  size_t capacity;     // offset to the stack end
  struct u7_vm_allocator allocator;
};

// Initializes the stack structure.
u7_error u7_vm_stack_init(struct u7_vm_stack* self, size_t capacity,
                          struct u7_vm_allocator allocator);

// Releases stack resources.
void u7_vm_stack_destroy(struct u7_vm_stack* self);

// Pushes a frame onto the stack. The stack treats return_ip as opaque and
// returns it unchanged from `u7_vm_stack_pop_frame()` when the frame is
// removed.
u7_error u7_vm_stack_push_frame(
    struct u7_vm_stack* self,
    struct u7_vm_stack_frame_layout const* frame_layout, size_t return_ip);

// Drops the trailing stack frame, returning the `return_ip` it was pushed
// with.
size_t u7_vm_stack_pop_frame(struct u7_vm_stack* self);

// Returns the current frame layout.
static inline struct u7_vm_stack_frame_layout const*
u7_vm_stack_current_frame_layout(struct u7_vm_stack* self) {
  assert(self->base_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + sizeof(struct u7_vm_stack_frame_header));
  return ((struct u7_vm_stack_frame_header const*)u7_vm_memory_add_offset(
              self->memory, self->base_offset))
      ->frame_layout;
}

// Returns a pointer to the globals.
static inline void* u7_vm_stack_globals(struct u7_vm_stack* self) {
  assert(self->top_offset >= U7_VM_STACK_FRAME_HEADER_SIZE);
  assert(self->top_offset >=
         U7_VM_STACK_FRAME_HEADER_SIZE +
             ((struct u7_vm_stack_frame_header const*)(self->memory))
                 ->frame_layout->locals_size);
  return u7_vm_memory_add_offset(self->memory, U7_VM_STACK_FRAME_HEADER_SIZE);
}

// Returns a pointer the current locals.
static inline void* u7_vm_stack_locals(struct u7_vm_stack* self) {
  assert(self->base_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size);
  return u7_vm_memory_add_offset(
      self->memory, self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE);
}

// False -- stops iteration.
typedef bool (*u7_vm_stack_visitor_fn_t)(
    void* data, struct u7_vm_stack_frame_layout const* frame_layout,
    void* frame_ptr);

// Iterates through the stack frames.
void u7_vm_stack_iterate(struct u7_vm_stack* self, void* data,
                         u7_vm_stack_visitor_fn_t visitor);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STACK_H_
