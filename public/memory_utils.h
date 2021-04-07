#ifndef U7_VM_MEMORY_UTILS_H_
#define U7_VM_MEMORY_UTILS_H_

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum {
  kU7VmDefaultAlignment = _Alignof(void*),
  kU7VmStandardAlignment = _Alignof(long double),
};

static inline size_t u7_vm_align_size(size_t size, int alignment) {
  return (size + alignment - 1) & (-(size_t)alignment);
}

// Returns the first aligned address.
static inline void* u7_vm_align_memory(void* ptr, int alignment) {
  return (void*)((((uintptr_t)ptr) + alignment - 1) & (-(uintptr_t)alignment));
}

// Indicates if the address is aligned.
static inline bool u7_vm_memory_is_aligned(void const* ptr, int alignment) {
  return (void*)((((uintptr_t)ptr) + alignment - 1) &
                 (((uintptr_t)alignment)) - 1);
}

// Adds an offset to the pointer.
#define u7_vm_memory_add_offset(ptr, offset) \
  _Generic((ptr),                                   \
           void*: u7_vm_memory_add_offset_v,        \
           const void*: u7_vm_memory_add_offset_vc  \
           )(ptr, offset)

static inline void* u7_vm_memory_add_offset_v(void* ptr, size_t offset) {
  return ((char*)ptr) + offset;
}

static inline const void* u7_vm_memory_add_offset_vc(void const* ptr,
                                                     size_t offset) {
  return ((char const*)ptr) + offset;
}

// Returns the number of bytes between `begin` and `end`.
//
// NOTE: The address in `begin` must not be after `end`, UB otherwise.
static inline size_t u7_vm_memory_byte_distance(void const* begin,
                                                void const* end) {
  assert(begin <= end);
  return ((const char*)end) - ((const char*)begin);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_MEMORY_UTILS_H_
