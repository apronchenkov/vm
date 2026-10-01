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
  self->ip = self->instructions;
  self->status = U7_VM_STATE_STATUS_READY;
  U7_RETURN_IF_ERROR(u7_vm_stack_init(
      &self->stack, options.initial_stack_capacity, options.allocator));
  struct u7_vm_stack_frame_cursor cursor =
      u7_vm_stack_load_frame_cursor(&self->stack);
  u7_error const error = u7_vm_stack_push_frame(
      &self->stack, options.statics_layout, self->instructions, &cursor);
  if (error.error_code != 0) {
    u7_vm_stack_destroy(&self->stack);
    return error;
  }
  u7_vm_stack_store_frame_cursor(&self->stack, cursor);
  return u7_ok();
}

void u7_vm_state_destroy(struct u7_vm_state* self) {
  u7_vm_stack_destroy(&self->stack);
}

// Unwinds frames and invokes their exception handlers.
// Returns true on recovery, with status set to RUNNING.
// Returns false if the exception remains unhandled at the root frame.
//
// The stack's stored position is current before each handler call and
// when this function returns. Handlers may modify the stack and must
// leave its stored position current.
static bool u7_vm_state_handle_exception(struct u7_vm_state* self) {
  assert(self->status == U7_VM_STATE_STATUS_EXCEPTION);
  struct u7_vm_stack_frame_cursor cursor =
      u7_vm_stack_load_frame_cursor(&self->stack);
  for (;;) {
    struct u7_vm_stack_frame_layout const* const frame_layout =
        u7_vm_stack_frame_layout(cursor.base);

    if (frame_layout->exception_handler_fn != NULL) {
      u7_vm_stack_store_frame_cursor(&self->stack, cursor);
      enum u7_vm_exception_handler_action const action =
          frame_layout->exception_handler_fn(
              self, frame_layout->exception_handler_data);
      if (action == U7_VM_EXCEPTION_HANDLER_ACTION_RECOVER) {
        assert(self->ip >= self->instructions &&
               self->ip < self->instructions + self->instructions_size);
        self->status = U7_VM_STATE_STATUS_RUNNING;
        return true;
      }
      cursor = u7_vm_stack_load_frame_cursor(&self->stack);
    }

    // Keep the root frame alive so globals remain accessible.
    if (cursor.base == self->stack.memory) {
      u7_vm_stack_store_frame_cursor(&self->stack, cursor);
      return false;
    }
    self->ip = u7_vm_stack_pop_frame(&cursor);
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
      assert(self->ip >= self->instructions &&
             self->ip < self->instructions + self->instructions_size);
    } while (U7_VM_INSTRUCTION_EXECUTE(self));
  } while (self->status == U7_VM_STATE_STATUS_EXCEPTION &&
           u7_vm_state_handle_exception(self));

  // A stopping instruction must set a non-running status.
  assert(self->status == U7_VM_STATE_STATUS_SUSPENDED ||
         self->status == U7_VM_STATE_STATUS_HALTED ||
         self->status == U7_VM_STATE_STATUS_EXCEPTION ||
         self->status == U7_VM_STATE_STATUS_CORRUPTED);
  return self->status;
}
