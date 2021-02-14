#ifndef U7_VM_STACK_H_
#define U7_VM_STACK_H_

#include "@/public/allocator.h"
#include "@/public/memory_layout.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_stack {
  void* base;
  void* begin;
  size_t size;
  size_t capacity;
  struct u7_vm_allocator* allocator;
};

// False -- stops iteration.
typedef bool (*u7_vm_stack_visitor_fn_t)(
    void* arg, struct u7_vm_memory_layout const* frame_layout, void* frame_ptr);

// Initializes the stack structure.
void u7_vm_stack_init(struct u7_vm_stack* self,
                      struct u7_vm_allocator* allocator);

// Releases stack resources.
void u7_vm_stack_destroy(struct u7_vm_stack* self);

// Reserves memory space in the stack.
bool u7_vm_stack_reserve(struct u7_vm_stack* self, size_t capacity);

// Returns a pointer to the new stack frame.
void* u7_vm_stack_push(struct u7_vm_stack* self,
                       struct u7_vm_memory_layout const* frame_layout);

// Drops the trailing stack frame.
void u7_vm_stack_pop(struct u7_vm_stack* self);

// Iterates through the stack frames.
void u7_vm_stack_iterate(struct u7_vm_stack* self, void* arg,
                         u7_vm_stack_visitor_fn_t visitor);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STACK_H_
