#ifndef U7_VM_INSTRUCTION_H_
#define U7_VM_INSTRUCTION_H_

#include <assert.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_state;

// Executes the instruction.
//
// Args:
//   data: Pointer to instruction-specific data.
//   state: Execution state.
//
// Returns:
//   False if the instruction chain should stop. The instruction is
//   responsible to update the execution state's status to indicate why.
typedef bool (*u7_vm_instruction_execute_fn_t)(void* data,
                                               struct u7_vm_state* state);

// Defines the interface to an instruction.
//
// This struct does not represent ownership. It may be copied, but the lifetime
// of `data` must be managed independently.
struct u7_vm_instruction {
  void* data;
  u7_vm_instruction_execute_fn_t execute_fn;
};

// Executes the instruction within the given state.
#define U7_VM_INSTRUCTION_EXECUTE(instr, state) \
  ((instr).execute_fn((instr).data, state))

// Defines an instruction's execute method `fn_name(self, state)`.
//
// If the instruction body returns false, execution stops immediately and
// `state->ip` is left unchanged. Otherwise, `state->ip` is advanced and
// execution continues with the next instruction.
//
// NOTE: Avoid using this macro for instructions that modify `state->ip`, such
// as jumps, calls, or returns. Doing so would result in multiple updates to
// `state->ip` and may hurt performance. Define such execute methods manually
// instead -- see U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT below.
#define U7_VM_DEFINE_INSTRUCTION_EXEC(fn_name, self_type)           \
  __attribute__((always_inline)) static inline bool fn_name##_impl( \
      self_type const* self, struct u7_vm_state* state);            \
                                                                    \
  static bool fn_name(void* data, struct u7_vm_state* state) {      \
    if (!fn_name##_impl((self_type const*)data, state)) {           \
      return false;                                                 \
    }                                                               \
    state->ip += 1;                                                 \
    assert(state->ip < state->instructions_size);                   \
    __attribute__((musttail)) return U7_VM_INSTRUCTION_EXECUTE(     \
        state->instructions[state->ip], state);                     \
  }                                                                 \
                                                                    \
  __attribute__((always_inline)) static inline bool fn_name##_impl( \
      __attribute__((unused)) self_type const* self,                \
      __attribute__((unused)) struct u7_vm_state* state)

// Defines an instruction's execute method `fn_name(self, state)` for
// instructions that manage `state->ip` explicitly.
//
// Unlike U7_VM_DEFINE_INSTRUCTION_EXEC, this macro does not advance
// `state->ip`. If the instruction body returns true, `state->ip` must point
// to the instruction to execute next. If the body returns false, execution
// stops and `state->ip` is left as set by the body.
//
// This is intended for jumps, calls, returns, and terminal instructions.
#define U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(fn_name, self_type)  \
  __attribute__((always_inline)) static inline bool fn_name##_impl( \
      self_type const* self, struct u7_vm_state* state);            \
                                                                    \
  static bool fn_name(void* data, struct u7_vm_state* state) {      \
    if (!fn_name##_impl((self_type const*)data, state)) {           \
      return false;                                                 \
    }                                                               \
    assert(state->ip < state->instructions_size);                   \
    __attribute__((musttail)) return U7_VM_INSTRUCTION_EXECUTE(     \
        state->instructions[state->ip], state);                     \
  }                                                                 \
                                                                    \
  __attribute__((always_inline)) static inline bool fn_name##_impl( \
      __attribute__((unused)) self_type const* self,                \
      __attribute__((unused)) struct u7_vm_state* state)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_INSTRUCTION_H_
