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

// Alignment of values in stack -> if we support stack growing, we cannot have
// alignment requirement > the allocator alignment.

struct u7_vm_state {
  u7_error error;
  struct u7_vm_instruction const** instructions;
  size_t instructions_size;
  size_t ip;
  struct u7_vm_stack stack;
};

u7_error u7_vm_state_init(struct u7_vm_state* self,
                          struct u7_vm_allocator* stack_allocator,
                          struct u7_vm_stack_frame_layout const* statics_layout,
                          struct u7_vm_instruction const** instructions,
                          size_t instructions_size);

void u7_vm_state_destroy(struct u7_vm_state* self);

void u7_vm_state_run(struct u7_vm_state* self);

void* u7_vm_state_statics(struct u7_vm_state* self);

static inline void u7_vm_instruction_enter(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)tail;
  (void)self;
  state->ip += 1;
}

static inline bool u7_vm_instruction_tail_call(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  (void)self;
  assert(state->ip < state->instructions_size);
  return u7_vm_instruction_execute(tail, state->instructions[state->ip], state);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STATE_H_
