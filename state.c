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
  self->status = U7_VM_STATE_STATUS_READY;
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

enum u7_vm_state_status u7_vm_state_run(struct u7_vm_state* self) {
  if (self->status == U7_VM_STATE_STATUS_HALTED ||
      self->status == U7_VM_STATE_STATUS_CORRUPTED) {
    return self->status;
  }

  assert(self->status != U7_VM_STATE_STATUS_RUNNING);
  self->status = U7_VM_STATE_STATUS_RUNNING;
  do {
    assert(self->ip < self->instructions_size);
  } while (U7_VM_INSTRUCTION_EXECUTE(self->instructions[self->ip], self));

  // A stopping instruction must set a non-running status.
  assert(self->status == U7_VM_STATE_STATUS_SUSPENDED ||
         self->status == U7_VM_STATE_STATUS_HALTED ||
         self->status == U7_VM_STATE_STATUS_CORRUPTED);
  return self->status;
}
