#ifndef U7_VM_ALLOCATOR_H_
#define U7_VM_ALLOCATOR_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Allocates `size` bytes of uninitialized memory.
//
// Args:
//   data: Pointer to allocator-specific data.
//   size: Number of bytes to allocate.
//
// Returns:
//   On success, a pointer to the beginning of the allocated memory.
//   On failure, NULL.
typedef void* (*u7_vm_allocator_allocate_fn_t)(void* data, size_t size);

// Reallocates a previously allocated memory area.
//
// Args:
//   data: Pointer to allocator-specific data.
//   memory: Pointer to the memory area to reallocate.
//   old_size: Current size of the memory area, in bytes.
//   new_size: Requested size of the memory area, in bytes.
//
// Returns:
//   On success, a pointer to the beginning of the reallocated memory.
//   On failure, NULL; memory remains valid.
typedef void* (*u7_vm_allocator_reallocate_fn_t)(void* data, void* memory,
                                                 size_t old_size,
                                                 size_t new_size);

// Releases a previously allocated memory area.
//
// Args:
//   data: Pointer to allocator-specific data.
//   memory: Pointer to the memory area to release.
//   size: Size of the memory area, in bytes.
typedef void (*u7_vm_allocator_deallocate_fn_t)(void* data, void* memory,
                                                size_t size);

// Defines the interface to an allocator.
//
// This struct does not represent ownership. It may be copied, but the lifetime
// of `data` must be managed independently.
struct u7_vm_allocator {
  void* data;
  u7_vm_allocator_allocate_fn_t allocate_fn;
  u7_vm_allocator_reallocate_fn_t reallocate_fn;
  u7_vm_allocator_deallocate_fn_t deallocate_fn;
};

// The default allocator backed by `malloc()`, `realloc()`, and `free()`.
extern struct u7_vm_allocator const u7_vm_default_allocator;

// An allocator that limits the total size of live allocations delegated to
// an upstream allocator.
//
// This type is not thread-safe.
struct u7_vm_limited_allocator {
  struct u7_vm_allocator upstream;
  size_t limit;
  size_t used;
};

// Initializes a limited allocator.
//
// Args:
//   self: Pointer to the limited allocator to initialize.
//   upstream: Allocator to which allocations are delegated.
//   limit: Maximum total size of live allocations, in bytes.
void u7_vm_limited_allocator_init(struct u7_vm_limited_allocator* self,
                                  struct u7_vm_allocator upstream,
                                  size_t limit);

// Returns an allocator interface backed by a limited allocator `self`.
//
// Importantly, `self` must outlive all allocations made through the returned
// interface.
//
// Args:
//   self: Pointer to the limited allocator.
//
// Returns:
//   An allocator interface backed by `self`.
struct u7_vm_allocator u7_vm_limited_allocator_make(
    struct u7_vm_limited_allocator* self);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_ALLOCATOR_H_
