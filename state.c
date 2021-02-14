#include "@/public/state.h"

#include <assert.h>

bool u7_vm_state_init(struct u7_vm_state* self,
                      struct u7_vm_allocator* stack_allocator,
                      struct u7_vm_memory_layout const* static_layout,
                      struct u7_vm_instruction const** instructions,
                      size_t instructions_size) {
  self->run = false;
  self->error = NULL;
  self->instructions = instructions;
  self->instructions_size = instructions_size;
  self->ip = 0;
  u7_vm_stack_init(&self->stack, stack_allocator);
  self->stack.base = u7_vm_stack_push(&self->stack, static_layout);
  if (!self->stack.base) {
    u7_vm_stack_destroy(&self->stack);
    return false;
  }
  return true;
}

void u7_vm_state_destroy(struct u7_vm_state* self) {
  u7_vm_stack_destroy(&self->stack);
}

void u7_vm_state_run(struct u7_vm_state* self) {
  if (self->error) {
    return;
  }
  for (self->run = true; self->run;) {
    assert(self->ip < self->instructions_size);
    u7_vm_instruction_execute(self->instructions[self->ip++], self);
  }
}
