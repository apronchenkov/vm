#ifndef U7_VM_MEMORY_LAYOUT_H_
#define U7_VM_MEMORY_LAYOUT_H_

#include "@/public/memory_utils.h"

#include <assert.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Description of a layout of a memory chunk.
//
// In particular, it provides procedures for in-memory values initialization
// and deconstruction.
struct u7_vm_memory_layout;

// Initializes the memory frame.
typedef void (*u7_vm_memory_layout_init_memory_fn_t)(
    struct u7_vm_memory_layout const* self, void* memory);

// Uninitializes the memory frame.
typedef void (*u7_vm_memory_layout_uninit_memory_fn_t)(
    struct u7_vm_memory_layout const* self, void* memory);

// Copies values to an initialized location.
typedef void (*u7_vm_memory_layout_copy_memory_fn_t)(
    struct u7_vm_memory_layout const* self, void* source, void* destination);

// Relocates values to uninitialized location.
typedef void (*u7_vm_memory_layout_relocate_memory_fn_t)(
    struct u7_vm_memory_layout const* self, void* source, void* destination);

// This struct declares usage API of Memory Layout.
//
// This struct doesn't represent ownership: no standard way to copy/move it, nor
// to destroy it.
struct u7_vm_memory_layout {
  int alignment;
  size_t size;
  u7_vm_memory_layout_init_memory_fn_t init_memory_fn;
  u7_vm_memory_layout_uninit_memory_fn_t uninit_memory_fn;
  u7_vm_memory_layout_relocate_memory_fn_t relocate_memory_fn;
  u7_vm_memory_layout_copy_memory_fn_t copy_memory_fn;
  const char* description;
};

// Returns the first address compatible with the layout in the given memory
// block.
static inline void* u7_vm_memory_layout_align_memory(
    struct u7_vm_memory_layout const* self, void* begin, void* end) {
  begin = u7_vm_align_memory(begin, self->alignment);
  if (begin > end || u7_vm_memory_byte_distance(begin, end) < self->size) {
    return NULL;
  }
  return begin;
}

// Initializes the given memory region.
static inline void u7_vm_memory_layout_init_memory(
    struct u7_vm_memory_layout const* self, void* memory) {
  assert(u7_vm_memory_is_aligned(memory, self->alignment));
  if (self->init_memory_fn) {
    self->init_memory_fn(self, memory);
  }
}

// Uninitialies the given memory region.
static inline void u7_vm_memory_layout_uninit_memory(
    struct u7_vm_memory_layout const* self, void* memory) {
  assert(u7_vm_memory_is_aligned(memory, self->alignment));
  if (self->uninit_memory_fn) {
    self->uninit_memory_fn(self, memory);
  }
}

// Copies values to an initialized location.
// Source address has to be initialized.
// Destination address must be initialized.
static inline void u7_vm_memory_layout_copy_memory(
    struct u7_vm_memory_layout const* self, void* source, void* destination) {
  assert(u7_vm_memory_is_aligned(source, self->alignment));
  assert(u7_vm_memory_is_aligned(destination, self->alignment));
  if (source != destination) {
    if (self->copy_memory_fn) {
      self->copy_memory_fn(self, source, destination);
    } else {
      memcpy(destination, source, self->size);
    }
  }
}

// Relocates values to uninitialized location.
// Source address has to be initialized.
// Destination address must be not initialized.
static inline void u7_vm_memory_layout_relocate_memory(
    struct u7_vm_memory_layout const* self, void* source, void* destination) {
  assert(u7_vm_memory_is_aligned(source, self->alignment));
  assert(u7_vm_memory_is_aligned(destination, self->alignment));
  if (source != destination) {
    if (self->relocate_memory_fn) {
      self->relocate_memory_fn(self, source, destination);
    } else {
      memcpy(destination, source, self->size);
    }
  }
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_MEMORY_LAYOUT_H_
