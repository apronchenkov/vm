#include "@/public/state.h"

#include <assert.h>

u7_error u7_vm_state_init(struct u7_vm_state* self,
                          struct u7_vm_stack_frame_layout const* statics_layout,
                          struct u7_vm_instruction const** instructions,
                          size_t instructions_size) {
  self->instructions = instructions;
  self->instructions_size = instructions_size;
  self->ip = 0;
  u7_vm_stack_init(&self->stack);
  return u7_vm_stack_push_frame(&self->stack, statics_layout);
}

void u7_vm_state_destroy(struct u7_vm_state* self) {
  u7_vm_stack_destroy(&self->stack);
}

void u7_vm_state_run(struct u7_vm_state* self) {
  const int kTail = 16;
  do {
    assert(self->ip < self->instructions_size);
  } while (
      u7_vm_instruction_execute(kTail, self->instructions[self->ip], self));
}
