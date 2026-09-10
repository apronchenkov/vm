#include "@/public/state.h"

#include <assert.h>

struct u7_vm_state_options u7_vm_state_options_default() {
  return (struct u7_vm_state_options){
      .initial_stack_capacity = 4096,
      .allocator = u7_vm_default_allocator,
  };
}

u7_error u7_vm_state_init(struct u7_vm_state* self,
                          struct u7_vm_state_options options) {
  self->instructions = options.instructions;
  self->instructions_size = options.instructions_size;
  self->ip = 0;
  U7_RETURN_IF_ERROR(u7_vm_stack_init(
      &self->stack, options.initial_stack_capacity, options.allocator));
  u7_error error = u7_vm_stack_push_frame(&self->stack, options.statics_layout);
  if (error.error_code != 0) {
    u7_vm_stack_destroy(&self->stack);
    return error;
  }
  return u7_ok();
}

void u7_vm_state_destroy(struct u7_vm_state* self) {
  u7_vm_stack_destroy(&self->stack);
}

void u7_vm_state_run(struct u7_vm_state* self) {
  do {
    assert(self->ip < self->instructions_size);
  } while (u7_vm_instruction_execute(self->instructions[self->ip], self));
}
