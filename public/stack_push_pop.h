#ifndef U7_VM_STACK_PUSH_POP_H_
#define U7_VM_STACK_PUSH_POP_H_

#include "@/public/stack.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Pushes a value to the stack.
static inline void u7_vm_stack_push_i32(struct u7_vm_stack* self,
                                        int32_t value) {
  assert(self->top_offset % kU7VmDefaultAlignment == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(value), kU7VmDefaultAlignment));
  *(int32_t*)u7_vm_memory_add_offset(self->memory, self->top_offset) = value;
  self->top_offset += u7_vm_align_size(sizeof(value), kU7VmDefaultAlignment);
}

// Pushes a value to the stack.
static inline void u7_vm_stack_push_i64(struct u7_vm_stack* self,
                                        int64_t value) {
  assert(self->top_offset % kU7VmDefaultAlignment == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(value), kU7VmDefaultAlignment));
  *(int64_t*)u7_vm_memory_add_offset(self->memory, self->top_offset) = value;
  self->top_offset += u7_vm_align_size(sizeof(value), kU7VmDefaultAlignment);
}

// Pushes a value to the stack.
static inline void u7_vm_stack_push_f32(struct u7_vm_stack* self, float value) {
  assert(self->top_offset % kU7VmDefaultAlignment == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(value), kU7VmDefaultAlignment));
  *(float*)u7_vm_memory_add_offset(self->memory, self->top_offset) = value;
  self->top_offset += u7_vm_align_size(sizeof(value), kU7VmDefaultAlignment);
}

// Pushes a value to the stack.
static inline void u7_vm_stack_push_f64(struct u7_vm_stack* self,
                                        double value) {
  assert(self->top_offset % kU7VmDefaultAlignment == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(value), kU7VmDefaultAlignment));
  *(double*)u7_vm_memory_add_offset(self->memory, self->top_offset) = value;
  self->top_offset += u7_vm_align_size(sizeof(value), kU7VmDefaultAlignment);
}

// Pushes a value to the stack.
static inline void u7_vm_stack_push_ptr(struct u7_vm_stack* self, void* ptr) {
  assert(self->top_offset % kU7VmDefaultAlignment == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(ptr), kU7VmDefaultAlignment));
  *(void**)u7_vm_memory_add_offset(self->memory, self->top_offset) = ptr;
  self->top_offset += u7_vm_align_size(sizeof(ptr), kU7VmDefaultAlignment);
}

// Pops a value from the stack.
static inline int32_t u7_vm_stack_pop_i32(struct u7_vm_stack* self) {
  assert(self->base_offset + kU7VmStackFrameHeaderSize +
             u7_vm_align_size(sizeof(int32_t), kU7VmDefaultAlignment) <=
         self->top_offset);
  self->top_offset -= u7_vm_align_size(sizeof(int32_t), kU7VmDefaultAlignment);
  return *(int32_t*)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

// Pops a value from the stack.
static inline int64_t u7_vm_stack_pop_i64(struct u7_vm_stack* self) {
  assert(self->base_offset + kU7VmStackFrameHeaderSize +
             u7_vm_align_size(sizeof(int64_t), kU7VmDefaultAlignment) <=
         self->top_offset);
  self->top_offset -= u7_vm_align_size(sizeof(int64_t), kU7VmDefaultAlignment);
  return *(int64_t*)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

// Pops a value from the stack.
static inline float u7_vm_stack_pop_f32(struct u7_vm_stack* self) {
  assert(self->base_offset + kU7VmStackFrameHeaderSize +
             u7_vm_align_size(sizeof(float), kU7VmDefaultAlignment) <=
         self->top_offset);
  self->top_offset -= u7_vm_align_size(sizeof(float), kU7VmDefaultAlignment);
  return *(float*)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

// Pops a value from the stack.
static inline double u7_vm_stack_pop_f64(struct u7_vm_stack* self) {
  assert(self->base_offset + kU7VmStackFrameHeaderSize +
             u7_vm_align_size(sizeof(double), kU7VmDefaultAlignment) <=
         self->top_offset);
  self->top_offset -= u7_vm_align_size(sizeof(double), kU7VmDefaultAlignment);
  return *(double*)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

// Pops a value from the stack.
static inline void* u7_vm_stack_pop_ptr(struct u7_vm_stack* self) {
  assert(self->base_offset + kU7VmStackFrameHeaderSize +
             u7_vm_align_size(sizeof(void*), kU7VmDefaultAlignment) <=
         self->top_offset);
  self->top_offset -= u7_vm_align_size(sizeof(void*), kU7VmDefaultAlignment);
  return *(void**)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STACK_PUSH_POP_H_