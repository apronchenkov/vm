#ifndef U7_VM_ALLOCATOR_H_
#define U7_VM_ALLOCATOR_H_

#include <github.com/apronchenkov/error/public/error.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_allocator;

// Allocates memory using the allocator.
typedef u7_error (*u7_vm_allocator_alloc_fn_t)(struct u7_vm_allocator* self,
                                               size_t size, void** result);

// Deallocates memory, previously allocated using the allocator.
typedef void (*u7_vm_allocator_free_fn_t)(struct u7_vm_allocator* self,
                                          void* ptr);

// This struct declares the usage API of a memory allocator.
//
// This struct doesn't represent ownership: no standard way to
// copy/move it, nor to destroy it.
struct u7_vm_allocator {
  int alignment;  // read-only
  u7_vm_allocator_alloc_fn_t alloc_fn;
  u7_vm_allocator_free_fn_t free_fn;
};

static inline u7_error u7_vm_allocator_alloc(struct u7_vm_allocator* self,
                                             size_t size, void** result) {
  return self->alloc_fn(self, size, result);
}

static inline void u7_vm_allocator_free(struct u7_vm_allocator* self,
                                        void* ptr) {
  self->free_fn(self, ptr);
}

struct u7_vm_allocator* u7_vm_default_allocator();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_ALLOCATOR_H_
