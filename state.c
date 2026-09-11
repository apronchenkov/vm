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
  u7_error error =
      u7_vm_stack_push_frame(&self->stack, options.statics_layout, 0);
  if (error.error_code != 0) {
    u7_vm_stack_destroy(&self->stack);
    return error;
  }
  return u7_ok();
}

void u7_vm_state_destroy(struct u7_vm_state* self) {
  u7_vm_stack_destroy(&self->stack);
}

// Unwinds frames and consults each frame's exception_handler_fn in turn.
// Returns true if a frame recovered, with status restored to RUNNING; returns
// false if the exception reached the root frame unhandled.
static bool u7_vm_state_handle_exception(struct u7_vm_state* self) {
  assert(self->status == U7_VM_STATE_STATUS_EXCEPTION);
  for (;;) {
    struct u7_vm_stack_frame_layout const* const frame_layout =
        u7_vm_stack_current_frame_layout(&self->stack);

    if (frame_layout->exception_handler_fn != NULL &&
        frame_layout->exception_handler_fn(
            self, frame_layout->exception_handler_data) ==
            U7_VM_EXCEPTION_HANDLER_ACTION_RECOVER) {
      assert(self->ip < self->instructions_size);
      self->status = U7_VM_STATE_STATUS_RUNNING;
      return true;
    }

    // Keep the root frame alive so globals remain accessible.
    if (self->stack.base_offset == 0) {
      return false;
    }
    self->ip = u7_vm_stack_pop_frame(&self->stack);
  }
}

enum u7_vm_state_status u7_vm_state_run(struct u7_vm_state* self) {
  if (self->status == U7_VM_STATE_STATUS_HALTED ||
      self->status == U7_VM_STATE_STATUS_EXCEPTION ||
      self->status == U7_VM_STATE_STATUS_CORRUPTED) {
    return self->status;
  }

  assert(self->status != U7_VM_STATE_STATUS_RUNNING);
  self->status = U7_VM_STATE_STATUS_RUNNING;
  do {
    do {
      assert(self->ip < self->instructions_size);
    } while (U7_VM_INSTRUCTION_EXECUTE(self->instructions[self->ip], self));
  } while (self->status == U7_VM_STATE_STATUS_EXCEPTION &&
           u7_vm_state_handle_exception(self));

  // A stopping instruction must set a non-running status.
  assert(self->status == U7_VM_STATE_STATUS_SUSPENDED ||
         self->status == U7_VM_STATE_STATUS_HALTED ||
         self->status == U7_VM_STATE_STATUS_EXCEPTION ||
         self->status == U7_VM_STATE_STATUS_CORRUPTED);
  return self->status;
}
