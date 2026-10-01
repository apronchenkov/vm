#ifndef U7_VM_STACK_PUSH_POP_H_
#define U7_VM_STACK_PUSH_POP_H_

#include "@/public/attributes.h"
#include "@/public/stack.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Defines typed push/pop operations that return an updated cursor.
// Push writes a value and advances the top. Pop retracts the top and
// reads the value into *out_value without clearing the stored bytes.
//
// Each value occupies a slot whose size is sizeof(c_type) rounded up to
// U7_VM_DEFAULT_ALIGNMENT. Bounds and alignment are checked with assertions.
// These operations do not update the stack's stored offsets.
#define U7_VM_DEFINE_STACK_FRAME_CURSOR_PUSH_POP(type, c_type)                 \
  U7_VM_ATTRIBUTE_NODISCARD static inline struct u7_vm_stack_frame_cursor      \
  u7_vm_stack_frame_cursor_push_##type(struct u7_vm_stack_frame_cursor cursor, \
                                       c_type value) {                         \
    struct u7_vm_stack_frame_cursor const new_cursor =                         \
        u7_vm_stack_frame_cursor_adjust_top(                                   \
            cursor, (ptrdiff_t)u7_vm_align_size(sizeof(c_type),                \
                                                U7_VM_DEFAULT_ALIGNMENT));     \
    assert(u7_vm_memory_is_aligned(cursor.top, U7_VM_DEFAULT_ALIGNMENT));      \
    *(c_type*)cursor.top = value;                                              \
    return new_cursor;                                                         \
  }                                                                            \
                                                                               \
  U7_VM_ATTRIBUTE_NODISCARD static inline struct u7_vm_stack_frame_cursor      \
  u7_vm_stack_frame_cursor_pop_##type(struct u7_vm_stack_frame_cursor cursor,  \
                                      c_type* out_value) {                     \
    struct u7_vm_stack_frame_cursor const new_cursor =                         \
        u7_vm_stack_frame_cursor_adjust_top(                                   \
            cursor, -(ptrdiff_t)u7_vm_align_size(sizeof(c_type),               \
                                                 U7_VM_DEFAULT_ALIGNMENT));    \
    assert(u7_vm_memory_is_aligned(new_cursor.top, U7_VM_DEFAULT_ALIGNMENT));  \
    *out_value = *(c_type*)new_cursor.top;                                     \
    return new_cursor;                                                         \
  }

U7_VM_DEFINE_STACK_FRAME_CURSOR_PUSH_POP(i32, int32_t)
U7_VM_DEFINE_STACK_FRAME_CURSOR_PUSH_POP(i64, int64_t)
U7_VM_DEFINE_STACK_FRAME_CURSOR_PUSH_POP(f32, float)
U7_VM_DEFINE_STACK_FRAME_CURSOR_PUSH_POP(f64, double)

#undef U7_VM_DEFINE_STACK_FRAME_CURSOR_PUSH_POP

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STACK_PUSH_POP_H_
