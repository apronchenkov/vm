#ifndef U7_VM_STATE_H_
#define U7_VM_STATE_H_

#include "@/public/instruction.h"
#include "@/public/stack.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Alignment of values in stack -> if we support rellocating, we cannot have
// alignment requirement > the allocator lignment.

struct u7_vm_state {
  bool run;
  const char* error;
  struct u7_vm_instruction const** instructions;
  size_t instructions_size;
  size_t ip;
  struct u7_vm_stack stack;
};

bool u7_vm_state_init(struct u7_vm_state* self,
                      struct u7_vm_allocator* stack_allocator,
                      struct u7_vm_memory_layout const* static_layout,
                      struct u7_vm_instruction const** instructions,
                      size_t instructions_size);

void u7_vm_state_destroy(struct u7_vm_state* self);

void u7_vm_state_run(struct u7_vm_state* self);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STATE_H_
