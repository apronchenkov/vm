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
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(value), U7_VM_DEFAULT_ALIGNMENT));
  *(int32_t*)u7_vm_memory_add_offset(self->memory, self->top_offset) = value;
  self->top_offset += u7_vm_align_size(sizeof(value), U7_VM_DEFAULT_ALIGNMENT);
}

// Pushes a value to the stack.
static inline void u7_vm_stack_push_i64(struct u7_vm_stack* self,
                                        int64_t value) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(value), U7_VM_DEFAULT_ALIGNMENT));
  *(int64_t*)u7_vm_memory_add_offset(self->memory, self->top_offset) = value;
  self->top_offset += u7_vm_align_size(sizeof(value), U7_VM_DEFAULT_ALIGNMENT);
}

// Pushes a value to the stack.
static inline void u7_vm_stack_push_f32(struct u7_vm_stack* self, float value) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(value), U7_VM_DEFAULT_ALIGNMENT));
  *(float*)u7_vm_memory_add_offset(self->memory, self->top_offset) = value;
  self->top_offset += u7_vm_align_size(sizeof(value), U7_VM_DEFAULT_ALIGNMENT);
}

// Pushes a value to the stack.
static inline void u7_vm_stack_push_f64(struct u7_vm_stack* self,
                                        double value) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->capacity >=
         self->top_offset +
             u7_vm_align_size(sizeof(value), U7_VM_DEFAULT_ALIGNMENT));
  *(double*)u7_vm_memory_add_offset(self->memory, self->top_offset) = value;
  self->top_offset += u7_vm_align_size(sizeof(value), U7_VM_DEFAULT_ALIGNMENT);
}

// Pops a value from the stack.
static inline int32_t u7_vm_stack_pop_i32(struct u7_vm_stack* self) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size +
             u7_vm_align_size(sizeof(int32_t), U7_VM_DEFAULT_ALIGNMENT));
  self->top_offset -=
      u7_vm_align_size(sizeof(int32_t), U7_VM_DEFAULT_ALIGNMENT);
  return *(int32_t*)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

// Pops a value from the stack.
static inline int64_t u7_vm_stack_pop_i64(struct u7_vm_stack* self) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size +
             u7_vm_align_size(sizeof(int64_t), U7_VM_DEFAULT_ALIGNMENT));
  self->top_offset -=
      u7_vm_align_size(sizeof(int64_t), U7_VM_DEFAULT_ALIGNMENT);
  return *(int64_t*)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

// Pops a value from the stack.
static inline float u7_vm_stack_pop_f32(struct u7_vm_stack* self) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size +
             u7_vm_align_size(sizeof(float), U7_VM_DEFAULT_ALIGNMENT));
  self->top_offset -= u7_vm_align_size(sizeof(float), U7_VM_DEFAULT_ALIGNMENT);
  return *(float*)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

// Pops a value from the stack.
static inline double u7_vm_stack_pop_f64(struct u7_vm_stack* self) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size +
             u7_vm_align_size(sizeof(double), U7_VM_DEFAULT_ALIGNMENT));
  self->top_offset -= u7_vm_align_size(sizeof(double), U7_VM_DEFAULT_ALIGNMENT);
  return *(double*)u7_vm_memory_add_offset(self->memory, self->top_offset);
}

// Peek the top stack value.
static inline int32_t* u7_vm_stack_peek_i32(struct u7_vm_stack* self) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size +
             u7_vm_align_size(sizeof(int32_t), U7_VM_DEFAULT_ALIGNMENT));
  return (int32_t*)u7_vm_memory_add_offset(
      self->memory,
      self->top_offset -
          u7_vm_align_size(sizeof(int32_t), U7_VM_DEFAULT_ALIGNMENT));
}

// Peek the top stack value.
static inline int64_t* u7_vm_stack_peek_i64(struct u7_vm_stack* self) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size +
             u7_vm_align_size(sizeof(int64_t), U7_VM_DEFAULT_ALIGNMENT));
  return (int64_t*)u7_vm_memory_add_offset(
      self->memory,
      self->top_offset -
          u7_vm_align_size(sizeof(int64_t), U7_VM_DEFAULT_ALIGNMENT));
}

// Peek the top stack value.
static inline float* u7_vm_stack_peek_f32(struct u7_vm_stack* self) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size +
             u7_vm_align_size(sizeof(float), U7_VM_DEFAULT_ALIGNMENT));
  return (float*)u7_vm_memory_add_offset(
      self->memory,
      self->top_offset -
          u7_vm_align_size(sizeof(float), U7_VM_DEFAULT_ALIGNMENT));
}

// Peek the top stack value.
static inline double* u7_vm_stack_peek_f64(struct u7_vm_stack* self) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(self->top_offset >=
         self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_current_frame_layout(self)->locals_size +
             u7_vm_align_size(sizeof(double), U7_VM_DEFAULT_ALIGNMENT));
  return (double*)u7_vm_memory_add_offset(
      self->memory,
      self->top_offset -
          u7_vm_align_size(sizeof(double), U7_VM_DEFAULT_ALIGNMENT));
}

// Duplicates a value on top of the stack.
static inline void u7_vm_stack_duplicate_i32(struct u7_vm_stack* self) {
  u7_vm_stack_push_i32(self, *u7_vm_stack_peek_i32(self));
}

// Duplicates a value on top of the stack.
static inline void u7_vm_stack_duplicate_i64(struct u7_vm_stack* self) {
  u7_vm_stack_push_i64(self, *u7_vm_stack_peek_i64(self));
}

// Duplicates a value on top of the stack.
static inline void u7_vm_stack_duplicate_f32(struct u7_vm_stack* self) {
  u7_vm_stack_push_f32(self, *u7_vm_stack_peek_f32(self));
}

// Duplicates a value on top of the stack.
static inline void u7_vm_stack_duplicate_f64(struct u7_vm_stack* self) {
  u7_vm_stack_push_f64(self, *u7_vm_stack_peek_f64(self));
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STACK_PUSH_POP_H_
