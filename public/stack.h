#ifndef U7_VM_STACK_H_
#define U7_VM_STACK_H_

#include "@/public/memory_utils.h"

#include <github.com/apronchenkov/error/public/error.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Procedures for a stack frame initialization and deconstruction.
struct u7_vm_stack_frame_layout;

// Initializes state for a stack frame.
typedef void (*u7_vm_stack_frame_layout_init_fn_t)(
    struct u7_vm_stack_frame_layout const* self, void* memory);

// Deinitializes state of a stack frame.
typedef void (*u7_vm_stack_frame_layout_deinit_fn_t)(
    struct u7_vm_stack_frame_layout const* self, void* memory);

// Post-processing after a reallocation of a stack frame a new memory address.
//
// NOTE: This function does postprocessing after realloc(), means it's safe to
// assume that "bytes" in source and destination has been already copied.
typedef void (*u7_vm_stack_frame_layout_post_realloc_fn_t)(
    struct u7_vm_stack_frame_layout const* self, void* source,
    void* destination);

// A stack frame layout (doesn't include the stack frame header).
struct u7_vm_stack_frame_layout {
  size_t locals_size;
  size_t extra_capacity;
  u7_vm_stack_frame_layout_init_fn_t init_fn;
  u7_vm_stack_frame_layout_deinit_fn_t deinit_fn;
  u7_vm_stack_frame_layout_post_realloc_fn_t post_realloc_fn;
  const char* description;
};

// A stack header of a stack frame.
struct u7_vm_stack_frame_header {
  size_t old_base_offset;
  struct u7_vm_stack_frame_layout const* frame_layout;
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
};

// False -- stops iteration.
typedef bool (*u7_vm_stack_visitor_fn_t)(
    void* arg, struct u7_vm_stack_frame_layout const* frame_layout,
    void* frame_ptr);

// Initializes the stack structure.
void u7_vm_stack_init(struct u7_vm_stack* self);

// Releases stack resources.
void u7_vm_stack_destroy(struct u7_vm_stack* self);

// Returns a pointer to the new stack frame.
u7_error u7_vm_stack_push_frame(
    struct u7_vm_stack* self,
    struct u7_vm_stack_frame_layout const* frame_layout);

// Drops the trailing stack frame.
void u7_vm_stack_pop_frame(struct u7_vm_stack* self);

// Iterates through the stack frames.
void u7_vm_stack_iterate(struct u7_vm_stack* self, void* arg,
                         u7_vm_stack_visitor_fn_t visitor);

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

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STACK_H_
