#ifndef U7_VM_INSTRUCTION_H_
#define U7_VM_INSTRUCTION_H_

#include <assert.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_state;
struct u7_vm_instruction;

// Executes the instruction.
typedef bool (*u7_vm_instruction_execute_fn_t)(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state);

// This struct declares the usage API of a VM Instruction.
//
// This struct doesn't represent ownership: no standard way to copy/move it, nor
// to destroy it.
struct u7_vm_instruction {
  u7_vm_instruction_execute_fn_t execute_fn;
};

// Executes the instruction within the given state.
#define u7_vm_instruction_execute(tail, self, state) \
  (tail == 0 || self->execute_fn(tail - 1, self, state))

#define u7_vm_instruction_enter(tail, self, state) \
  do {                                             \
    (void)tail;                                    \
    (void)self;                                    \
    state->ip += 1;                                \
  } while (false)

#define u7_vm_instruction_tail_call(tail, self, state)       \
  ((void)self, assert(state->ip < state->instructions_size), \
   u7_vm_instruction_execute(tail, state->instructions[state->ip], state))

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_INSTRUCTION_H_
