#ifndef U7_VM_STATE_H_
#define U7_VM_STATE_H_

#include "@/public/instruction.h"
#include "@/public/stack.h"

#include <github.com/apronchenkov/error/public/error.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_state {
  struct u7_vm_instruction const** instructions;
  size_t instructions_size;
  size_t ip;
  struct u7_vm_stack stack;
};

u7_error u7_vm_state_init(struct u7_vm_state* self,
                          struct u7_vm_stack_frame_layout const* statics_layout,
                          struct u7_vm_instruction const** instructions,
                          size_t instructions_size);

void u7_vm_state_destroy(struct u7_vm_state* self);

void u7_vm_state_run(struct u7_vm_state* self);

static inline void* u7_vm_state_globals(struct u7_vm_state* self) {
  return u7_vm_stack_globals(&self->stack);
}

static inline void* u7_vm_state_locals(struct u7_vm_state* self) {
  return u7_vm_stack_locals(&self->stack);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STATE_H_
