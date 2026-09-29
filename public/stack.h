#ifndef U7_VM_STACK_H_
#define U7_VM_STACK_H_

#include "@/public/allocator.h"
#include "@/public/memory_utils.h"

#include <errno.h>
#include <github.com/apronchenkov/u7_init/public/init.h>
#include <github.com/apronchenkov/u7_init/public/math.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_state;
struct u7_vm_instruction;

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
  struct u7_vm_stack_frame_layout const* /*nonnull*/ frame_layout;
  struct u7_vm_instruction const* /*nullable*/ return_ip;
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

// Ensures that the stack can hold at least `required_capacity` bytes
// without reallocation. If the current capacity is smaller, new storage
// is allocated; otherwise, this function does nothing.
u7_error u7_vm_stack_reserve(struct u7_vm_stack* self,
                             size_t required_capacity);

// Pushes a frame onto the stack. The stack treats `return_ip` as opaque
// and returns it unchanged from `u7_vm_stack_pop_frame()` when the frame
// is removed.
static inline u7_error u7_vm_stack_push_frame(
    struct u7_vm_stack* self, struct u7_vm_stack_frame_layout const* frame_layout,
    struct u7_vm_instruction const* return_ip) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(frame_layout->locals_size % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset <= self->capacity);

  bool overflow = false;
  const size_t new_top_offset = U7_ADD_OVERFLOW_U64(
      self->top_offset,
      U7_ADD_OVERFLOW_U64((size_t)U7_VM_STACK_FRAME_HEADER_SIZE,
                          frame_layout->locals_size, &overflow),
      &overflow);
  const size_t required_capacity = U7_ADD_OVERFLOW_U64(
      new_top_offset, frame_layout->extra_capacity, &overflow);
  if (overflow) {
    return u7_errnof(EOVERFLOW, "u7_vm_stack_push_frame: size overflow");
  }
  U7_RETURN_IF_ERROR(u7_vm_stack_reserve(self, required_capacity));

  struct u7_vm_stack_frame_header* const frame_header =
      (struct u7_vm_stack_frame_header*)u7_vm_memory_add_offset(
          self->memory, self->top_offset);
  frame_header->old_base_offset = self->base_offset;
  frame_header->frame_layout = frame_layout;
  frame_header->return_ip = return_ip;
  if (frame_layout->init_fn) {
    frame_layout->init_fn(
        frame_layout,
        u7_vm_memory_add_offset(
            self->memory, self->top_offset + U7_VM_STACK_FRAME_HEADER_SIZE));
  }
  self->base_offset = self->top_offset;
  self->top_offset = new_top_offset;
  return u7_ok();
}

// Drops the trailing stack frame, returning the `return_ip` it was pushed
// with.
static inline struct u7_vm_instruction const* u7_vm_stack_pop_frame(
    struct u7_vm_stack* self) {
  size_t base_offset = self->base_offset;
  size_t top_offset = self->top_offset;
  (void)top_offset;
  assert(base_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(top_offset >= base_offset + sizeof(struct u7_vm_stack_frame_header));
  struct u7_vm_stack_frame_header const frame_header =
      *(struct u7_vm_stack_frame_header*)u7_vm_memory_add_offset(self->memory,
                                                                 base_offset);
  struct u7_vm_stack_frame_layout const* const frame_layout =
      frame_header.frame_layout;
  assert(top_offset >=
         base_offset + U7_VM_DEFAULT_ALIGNMENT + frame_layout->locals_size);
  if (frame_layout->deinit_fn) {
    frame_layout->deinit_fn(
        frame_layout,
        u7_vm_memory_add_offset(
            self->memory, self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE));
  }
  self->top_offset = self->base_offset;
  self->base_offset = frame_header.old_base_offset;
  return frame_header.return_ip;
}

// Returns a pointer to the current frame's base (the start of its header).
static inline void* u7_vm_stack_frame_base(struct u7_vm_stack* self) {
  assert(self->base_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  return u7_vm_memory_add_offset(self->memory, self->base_offset);
}

// Returns the layout of the frame at `base`.
static inline struct u7_vm_stack_frame_layout const*
u7_vm_stack_frame_layout_at(void* base) {
  assert(u7_vm_memory_is_aligned(base, U7_VM_DEFAULT_ALIGNMENT));
  return ((struct u7_vm_stack_frame_header const*)base)->frame_layout;
}

// Returns a pointer to the locals of the frame at `base`.
static inline void* u7_vm_stack_locals_at(void* base) {
  assert(u7_vm_memory_is_aligned(base, U7_VM_DEFAULT_ALIGNMENT));
  return u7_vm_memory_add_offset(base, U7_VM_STACK_FRAME_HEADER_SIZE);
}

// Returns the current frame layout.
static inline struct u7_vm_stack_frame_layout const*
u7_vm_stack_current_frame_layout(struct u7_vm_stack* self) {
  assert(self->top_offset >=
         self->base_offset + sizeof(struct u7_vm_stack_frame_header));
  return u7_vm_stack_frame_layout_at(u7_vm_stack_frame_base(self));
}

// Returns a pointer to the globals.
static inline void* u7_vm_stack_globals(struct u7_vm_stack* self) {
  assert(self->top_offset >= U7_VM_STACK_FRAME_HEADER_SIZE);
  assert(self->top_offset >=
         U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_frame_layout_at(self->memory)->locals_size);
  return u7_vm_stack_locals_at(self->memory);
}

// Returns a pointer the current locals.
static inline void* u7_vm_stack_locals(struct u7_vm_stack* self) {
  void* const base = u7_vm_stack_frame_base(self);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_frame_layout_at(base)->locals_size);
  return u7_vm_stack_locals_at(base);
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
