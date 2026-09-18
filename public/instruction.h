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
//   data: Pointer to instruction-specific data.
//   state: Execution state.
//   ip: Pointer to the instruction being executed.
//
// Returns:
//   False if the instruction chain should stop. The instruction is
//   responsible to update the execution state's status to indicate why.
typedef bool (*u7_vm_instruction_execute_fn_t)(
    void* data, struct u7_vm_state* state,
    struct u7_vm_instruction const* ip);

// Defines the interface to an instruction.
//
// This struct does not represent ownership. It may be copied, but the lifetime
// of `data` must be managed independently.
struct u7_vm_instruction {
  void* data;
  u7_vm_instruction_execute_fn_t execute_fn;
};

// Executes the instruction at `ip` within the given state.
#define U7_VM_INSTRUCTION_EXECUTE(ip, state) \
  ((ip)->execute_fn((ip)->data, state, ip))

// Defines an instruction executor whose body receives `self` and `state`.
//
// Return true to chain execution to the next instruction, or false to
// stop at the current instruction. On stopping, `state->ip` is set to
// the current instruction.
//
// The body must not modify `state->ip` or rely on it identifying the
// current instruction.
//
// For instructions that need their current position or control where
// execution continues or stops, use
// U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT below.
#define U7_VM_DEFINE_INSTRUCTION_EXEC(fn_name, self_type)               \
  __attribute__((always_inline)) static inline bool fn_name##_impl(     \
      self_type const* self, struct u7_vm_state* state);                \
                                                                        \
  static bool fn_name(void* data, struct u7_vm_state* state,            \
                      struct u7_vm_instruction const* ip) {             \
    if (!fn_name##_impl((self_type const*)data, state)) {               \
      state->ip = ip;                                                   \
      return false;                                                     \
    }                                                                   \
    struct u7_vm_instruction const* const next_ip = ip + 1;             \
    assert(next_ip < state->instructions + state->instructions_size);   \
    __attribute__((musttail)) return U7_VM_INSTRUCTION_EXECUTE(next_ip, \
                                                               state);  \
  }                                                                     \
                                                                        \
  __attribute__((always_inline)) static inline bool fn_name##_impl(     \
      __attribute__((unused)) self_type const* self,                    \
      __attribute__((unused)) struct u7_vm_state* state)

// Defines an instruction executor whose body receives `self`, `state`,
// and `ip`, where `ip` points to the current instruction.
//
// Unlike U7_VM_DEFINE_INSTRUCTION_EXEC, this macro lets the body choose
// where execution continues or stops.
//
// Return `ip + 1` to chain execution to the next instruction, or another
// instruction pointer to transfer control elsewhere. If the body returns
// a non-NULL pointer, it must leave `state->ip` unchanged.
//
// Return NULL to stop the instruction chain. Before returning NULL,
// set `state->ip` to the appropriate stopping position. For example,
// yield sets it to `ip + 1` so the next call to u7_vm_state_run resumes
// after it.
//
// Use this macro for jumps, calls, returns, yield, and terminal
// instructions.
#define U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(fn_name, self_type)             \
  __attribute__((always_inline)) static inline struct u7_vm_instruction const* \
  fn_name##_impl(self_type const* self, struct u7_vm_state* state,             \
                 struct u7_vm_instruction const* ip);                          \
                                                                               \
  static bool fn_name(void* data, struct u7_vm_state* state,                   \
                      struct u7_vm_instruction const* ip) {                    \
    struct u7_vm_instruction const* const next_ip =                            \
        fn_name##_impl((self_type const*)data, state, ip);                     \
    if (next_ip == NULL) {                                                     \
      return false;                                                            \
    }                                                                          \
    assert(next_ip < state->instructions + state->instructions_size);          \
    __attribute__((musttail)) return U7_VM_INSTRUCTION_EXECUTE(next_ip,        \
                                                               state);         \
  }                                                                            \
                                                                               \
  __attribute__((always_inline)) static inline struct u7_vm_instruction const* \
  fn_name##_impl(__attribute__((unused)) self_type const* self,                \
                 __attribute__((unused)) struct u7_vm_state* state,            \
                 __attribute__((unused)) struct u7_vm_instruction const* ip)

// Defines an instruction executor whose body receives `self` and `state`,
// with a separate cold failure path.
//
// Return true to chain execution to the next instruction, as with
// U7_VM_DEFINE_INSTRUCTION_EXEC. Return false to tail-call
// `fn_name##_failure`, which sets `state->ip` to the current instruction
// before running the failure body.
//
// The executor body must not modify `state->ip` or rely on it identifying
// the current instruction.
//
// Define the failure function with
// U7_VM_DEFINE_INSTRUCTION_FAILURE_FN(fn_name, self_type)
// before using this macro.
//
// Keeping the failure path in a separate function is intended to reduce
// register-preservation overhead on the normal execution path.
#define U7_VM_DEFINE_INSTRUCTION_EXEC_WITH_COLD_FAILURE(fn_name, self_type) \
  __attribute__((always_inline)) static inline bool fn_name##_impl(         \
      self_type const* self, struct u7_vm_state* state);                    \
                                                                            \
  __attribute__((cold, noinline)) static bool fn_name##_failure(            \
      void* data, struct u7_vm_state* state,                                \
      struct u7_vm_instruction const* ip);                                  \
                                                                            \
  static bool fn_name(void* data, struct u7_vm_state* state,                \
                      struct u7_vm_instruction const* ip) {                 \
    if (__builtin_expect(!fn_name##_impl((self_type const*)data, state),    \
                         false)) {                                          \
      __attribute__((musttail)) return fn_name##_failure(data, state, ip);  \
    }                                                                       \
    struct u7_vm_instruction const* const next_ip = ip + 1;                 \
    assert(next_ip < state->instructions + state->instructions_size);       \
    __attribute__((musttail)) return U7_VM_INSTRUCTION_EXECUTE(next_ip,     \
                                                               state);      \
  }                                                                         \
                                                                            \
  __attribute__((always_inline)) static inline bool fn_name##_impl(         \
      __attribute__((unused)) self_type const* self,                        \
      __attribute__((unused)) struct u7_vm_state* state)

// Defines the cold failure function for an instruction executor defined
// with U7_VM_DEFINE_INSTRUCTION_EXEC_WITH_COLD_FAILURE.
//
// The body receives `self` and `state`. Before the body runs, `state->ip`
// is set to the current instruction.
//
// Return false to stop the instruction chain. Leave `state->ip` unchanged
// to stop at the current instruction, or set it to another position if
// required by the instruction's semantics. The body must always return
// false; returning true here is undefined -- nothing computes a next
// instruction to dispatch to on this path.
#define U7_VM_DEFINE_INSTRUCTION_FAILURE_FN(fn_name, self_type)    \
  __attribute__((cold)) static inline bool fn_name##_failure_impl( \
      self_type const* self, struct u7_vm_state* state);           \
                                                                   \
  __attribute__((cold, noinline)) static bool fn_name##_failure(   \
      void* data, struct u7_vm_state* state,                       \
      struct u7_vm_instruction const* ip) {                        \
    state->ip = ip;                                                \
    return fn_name##_failure_impl((self_type const*)data, state);  \
  }                                                                \
                                                                   \
  __attribute__((cold)) static inline bool fn_name##_failure_impl( \
      __attribute__((unused)) self_type const* self,               \
      __attribute__((unused)) struct u7_vm_state* state)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_INSTRUCTION_H_
