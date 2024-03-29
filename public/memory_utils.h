#ifndef U7_VM_MEMORY_UTILS_H_
#define U7_VM_MEMORY_UTILS_H_

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifdef __cplusplus
#define u7_vm_alignof(T) (alignof(T))
#else
#define u7_vm_alignof(T) (_Alignof(T))
#endif  // __cplusplus

#ifdef __cplusplus
#define u7_vm_offsetof(st, m) (offsetof(st, m))
#else
#define u7_vm_offsetof(st, m) ((size_t)((char*)&((st*)NULL)->m - (char*)NULL))
#endif  // __cplusplus

#define u7_vm_align_size(size, alignment) \
  (((size_t)(size) + (size_t)(alignment)-1) & (-(size_t)alignment))

enum {
  U7_VM_DEFAULT_ALIGNMENT = u7_vm_alignof(void*),
};

// Returns the first aligned address.
static inline void* u7_vm_align_memory(void* ptr, int alignment) {
  return (void*)((((uintptr_t)ptr) + alignment - 1) & (-(uintptr_t)alignment));
}

// Indicates if the address is aligned.
static inline bool u7_vm_memory_is_aligned(void const* ptr, int alignment) {
  return ((uintptr_t)ptr) % ((uintptr_t)alignment) == 0;
}

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

// Adds an offset to the pointer.
static inline void* u7_vm_memory_add_offset(void* ptr, size_t offset) {
  return u7_vm_memory_add_offset_v(ptr, offset);
}
static inline const void* u7_vm_memory_add_offset(void const* ptr,
                                                  size_t offset) {
  return u7_vm_memory_add_offset_vc(ptr, offset);
}

#else

// Adds an offset to the pointer.
#define u7_vm_memory_add_offset(ptr, offset) \
  _Generic((ptr),                                   \
           void*: u7_vm_memory_add_offset_v,        \
           const void*: u7_vm_memory_add_offset_vc  \
           )(ptr, offset)
#endif  // __cplusplus

#endif  // U7_VM_MEMORY_UTILS_H_
