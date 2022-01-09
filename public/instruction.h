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
//
// Args:
//   tail: Tail counter; zero value signals that the instruction should avoid
//     calling the next instrution internally.
//   self: Pointer to the instruction struct.
//   state: Execution state.
//
// Returns `false` indicates that the execution should be stoped.
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

// Helper macro.
#define U7_VM_DEFINE_INSTRUCTION_EXEC(fn_name, self_type)                  \
  __attribute__((always_inline)) static inline bool fn_name##_impl(        \
      self_type const* self, struct u7_vm_state* state);                   \
                                                                           \
  static bool fn_name(int tail, struct u7_vm_instruction const* self,      \
                      struct u7_vm_state* state) {                         \
    state->ip += 1;                                                        \
    if (!fn_name##_impl((self_type const*)self, state)) {                  \
      return false;                                                        \
    }                                                                      \
    assert(state->ip < state->instructions_size);                          \
    return u7_vm_instruction_execute(tail, state->instructions[state->ip], \
                                     state);                               \
  }                                                                        \
                                                                           \
  __attribute__((always_inline)) static inline bool fn_name##_impl(        \
      __attribute__((unused)) self_type const* self,                       \
      __attribute__((unused)) struct u7_vm_state* state)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_INSTRUCTION_H_
