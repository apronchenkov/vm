#ifndef U7_VM_INSTRUCTION_H_
#define U7_VM_INSTRUCTION_H_

#include <assert.h>
#include <github.com/apronchenkov/u7_init/public/optimization.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_state;
struct u7_vm_instruction;

// Use a calling convention that preserves fewer general-purpose registers,
// reducing register save/restore overhead in the instruction dispatch chain.
//
// Use the ordinary calling convention when the attribute is unavailable.
#if defined(__has_attribute) && __has_attribute(preserve_none)
#define U7_VM_INSTRUCTION_EXEC_CALL_CONV __attribute__((preserve_none))
#else
#define U7_VM_INSTRUCTION_EXEC_CALL_CONV
#endif

// Executes the instruction.
//
// Args:
//   data: Pointer to instruction-specific data.
//   state: Execution state.
//   ip: Pointer to the instruction being executed.
//   base: base: Pointer to the current frame's base.
//
// Returns:
//   False if the instruction chain should stop. The instruction is
//   responsible to update the execution state's status to indicate why.
typedef bool (*u7_vm_instruction_execute_fn_t)(
    void* data, struct u7_vm_state* state, struct u7_vm_instruction const* ip,
    void* base) U7_VM_INSTRUCTION_EXEC_CALL_CONV;

// Defines the interface to an instruction.
//
// This struct does not represent ownership. It may be copied, but the lifetime
// of `data` must be managed independently.
struct u7_vm_instruction {
  void* data;
  u7_vm_instruction_execute_fn_t execute_fn;
};

// Executes the instruction at `ip` using `state` and the current frame's
// base pointer. `base` must equal `u7_vm_stack_frame_base(&state->stack)`.
#define U7_VM_INSTRUCTION_EXECUTE(ip, state, base) \
  ((ip)->execute_fn((ip)->data, state, ip, base))

// Defines an instruction's execution function. The body receives `self`,
// `state`, and `base`.
//
// Return true to chain execution to the next instruction, forwarding
// `base` unchanged. Return false to stop at the current instruction;
// `state->ip` is then set to that instruction.
//
// The body must not modify `state->ip` or rely on it identifying the
// current instruction. It may push or pop stack values without relocating
// the stack. It must not push or pop stack frames.
//
// For instructions that need their current position, control where
// execution continues or stops, change the current frame, or relocate
// the stack, define the execution function manually or use
// U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT below.
#define U7_VM_DEFINE_INSTRUCTION_EXEC(fn_name, self_type)                      \
  __attribute__((always_inline)) static inline bool fn_name##_impl(            \
      self_type const* self, struct u7_vm_state* state, void* base);           \
                                                                               \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV static bool fn_name(                        \
      void* data, struct u7_vm_state* state,                                   \
      struct u7_vm_instruction const* ip, void* base) {                        \
    if (!fn_name##_impl((self_type const*)data, state, base)) {                \
      state->ip = ip;                                                          \
      return false;                                                            \
    }                                                                          \
    struct u7_vm_instruction const* const next_ip = ip + 1;                    \
    assert(next_ip < state->instructions + state->instructions_size);          \
    __attribute__((musttail)) return U7_VM_INSTRUCTION_EXECUTE(next_ip, state, \
                                                               base);          \
  }                                                                            \
                                                                               \
  __attribute__((always_inline)) static inline bool fn_name##_impl(            \
      __attribute__((unused)) self_type const* self,                           \
      __attribute__((unused)) struct u7_vm_state* state,                       \
      __attribute__((unused)) void* base)

// Result of a U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT body.
struct u7_vm_instruction_exec_explicit_result {
  struct u7_vm_instruction const* /*nonnull*/ ip;  // Next or stopping position.
  void* /*nullable*/ base;                         // Ignored when stopping.
  bool stop;
};

// Constructs a result for a U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT body.
// `.stop` defaults to false. `.ip` is required for both continuation and
// stopping; `.base` is required only for continuation.
#define U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT(...) \
  ((struct u7_vm_instruction_exec_explicit_result){__VA_ARGS__})

// Defines an instruction's execution function. The body receives `self`,
// `state`, `ip`, and `base`, where `ip` points to the current instruction
// and `base` points to the current frame's base.
//
// Unlike U7_VM_DEFINE_INSTRUCTION_EXEC, this macro lets the body choose
// where execution continues or stops.
//
// Return U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT with `.ip` and `.base`
// specifying the next instruction and its frame base. Use
// `.ip = ip + 1, .base = base` to execute the following instruction
// in the same frame.
//
// After changing frames or relocating the stack, recompute `base` before
// using it again or returning it for further execution.
//
// Set `.stop = true` to stop the instruction chain. The returned `ip`
// is stored in `state->ip`, and `base` is ignored. For example, yield
// returns `.ip = ip + 1, .stop = true` to resume after itself.
//
// The body must not modify `state->ip` or rely on it identifying the
// current instruction.
#define U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(fn_name, self_type)             \
  __attribute__((always_inline)) static inline struct                          \
      u7_vm_instruction_exec_explicit_result                                   \
      fn_name##_impl(self_type const* self, struct u7_vm_state* state,         \
                     struct u7_vm_instruction const* ip, void* base);          \
                                                                               \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV static bool fn_name(                        \
      void* data, struct u7_vm_state* state,                                   \
      struct u7_vm_instruction const* ip, void* base) {                        \
    struct u7_vm_instruction_exec_explicit_result const next =                 \
        fn_name##_impl((self_type const*)data, state, ip, base);               \
    assert(next.ip != NULL);                                                   \
    assert(next.ip >= state->instructions &&                                   \
           next.ip < state->instructions + state->instructions_size);          \
    if (U7_UNLIKELY(next.stop)) {                                              \
      state->ip = next.ip;                                                     \
      return false;                                                            \
    }                                                                          \
    __attribute__((musttail)) return U7_VM_INSTRUCTION_EXECUTE(next.ip, state, \
                                                               next.base);     \
  }                                                                            \
                                                                               \
  __attribute__((always_inline)) static inline struct                          \
      u7_vm_instruction_exec_explicit_result                                   \
      fn_name##_impl(                                                          \
          __attribute__((unused)) self_type const* self,                       \
          __attribute__((unused)) struct u7_vm_state* state,                   \
          __attribute__((unused)) struct u7_vm_instruction const* ip,          \
          __attribute__((unused)) void* base)

// Defines an instruction's execution function with a separate cold failure
// path. The body receives `self`, `state`, and `base`.
//
// Return true to chain execution to the next instruction. Return false to
// tail-call the failure function, which sets `state->ip` to the current
// instruction, runs the failure body, and stops the instruction chain.
//
// The body must not modify `state->ip` or rely on it identifying the
// current instruction. If the body returns true, execution continues
// with the instruction immediately following the current one.
//
// `base` is forwarded unchanged to the next instruction or failure
// function. The body may push or pop stack values without relocating
// the stack. It must not push or pop stack frames.
//
// Define the failure function with
// U7_VM_DEFINE_INSTRUCTION_FAILURE_FN(fn_name, self_type)
// before using this macro.
#define U7_VM_DEFINE_INSTRUCTION_EXEC_WITH_COLD_FAILURE(fn_name, self_type)    \
  __attribute__((always_inline)) static inline bool fn_name##_impl(            \
      self_type const* self, struct u7_vm_state* state, void* base);           \
                                                                               \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV                                             \
  __attribute__((cold, noinline)) static bool fn_name##_failure(               \
      void* data, struct u7_vm_state* state,                                   \
      struct u7_vm_instruction const* ip, void* base);                         \
                                                                               \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV static bool fn_name(                        \
      void* data, struct u7_vm_state* state,                                   \
      struct u7_vm_instruction const* ip, void* base) {                        \
    if (__builtin_expect(!fn_name##_impl((self_type const*)data, state, base), \
                         false)) {                                             \
      __attribute__((musttail)) return fn_name##_failure(data, state, ip,      \
                                                         base);                \
    }                                                                          \
    struct u7_vm_instruction const* const next_ip = ip + 1;                    \
    assert(next_ip < state->instructions + state->instructions_size);          \
    __attribute__((musttail)) return U7_VM_INSTRUCTION_EXECUTE(next_ip, state, \
                                                               base);          \
  }                                                                            \
                                                                               \
  __attribute__((always_inline)) static inline bool fn_name##_impl(            \
      __attribute__((unused)) self_type const* self,                           \
      __attribute__((unused)) struct u7_vm_state* state,                       \
      __attribute__((unused)) void* base)

// Defines the cold failure function for an instruction defined with
// U7_VM_DEFINE_INSTRUCTION_EXEC_WITH_COLD_FAILURE.
//
// The body receives `self`, `state`, and `base`. On entry to the body,
// `state->ip` points to the current instruction.
//
// The body returns void and must set the execution status to indicate
// the failure. It may adjust `state->ip` if a different stopping position
// is required. The instruction chain stops when the body returns.
#define U7_VM_DEFINE_INSTRUCTION_FAILURE_FN(fn_name, self_type)      \
  __attribute__((cold)) static inline void fn_name##_failure_impl(   \
      self_type const* self, struct u7_vm_state* state, void* base); \
                                                                     \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV                                   \
  __attribute__((cold, noinline)) static bool fn_name##_failure(     \
      void* data, struct u7_vm_state* state,                         \
      struct u7_vm_instruction const* ip, void* base) {              \
    state->ip = ip;                                                  \
    fn_name##_failure_impl((self_type const*)data, state, base);     \
    return false;                                                    \
  }                                                                  \
                                                                     \
  __attribute__((cold)) static inline void fn_name##_failure_impl(   \
      __attribute__((unused)) self_type const* self,                 \
      __attribute__((unused)) struct u7_vm_state* state,             \
      __attribute__((unused)) void* base)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_INSTRUCTION_H_
