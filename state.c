#include "@/public/state.h"

#include <assert.h>

u7_error u7_vm_state_init(struct u7_vm_state* self,
                          struct u7_vm_allocator* stack_allocator,
                          struct u7_vm_stack_frame_layout const* statics_layout,
                          struct u7_vm_instruction const** instructions,
                          size_t instructions_size) {
  self->error = u7_ok();
  self->instructions = instructions;
  self->instructions_size = instructions_size;
  self->ip = 0;
  u7_vm_stack_init(&self->stack, stack_allocator);
  return u7_vm_stack_push_frame(&self->stack, statics_layout);
}

void u7_vm_state_destroy(struct u7_vm_state* self) {
  u7_vm_stack_destroy(&self->stack);
  u7_error_release(self->error);
}

void u7_vm_state_run(struct u7_vm_state* self) {
  const int kTail = 16;
  if (!self->error.error_code) {
    do {
      assert(self->ip < self->instructions_size);
    } while (
        u7_vm_instruction_execute(kTail, self->instructions[self->ip], self));
  }
}

void* u7_vm_state_static_frame(struct u7_vm_state* self) {
  return u7_vm_memory_add_offset(self->stack.memory,
                                 U7_VM_STACK_FRAME_HEADER_SIZE);
}
