#ifndef U7_VM_INSTRUCTION_H_
#define U7_VM_INSTRUCTION_H_

#include "@/public/attributes.h"
#include "@/public/stack.h"

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
#define U7_VM_INSTRUCTION_EXEC_CALL_CONV U7_VM_ATTRIBUTE_PRESERVE_NONE

// Executes the instruction.
//
// The instruction is responsible for updating state (`ip` and `stack`) before
// returning. It may delegate this responsibility by tail-calling another
// instruction's `execute_fn`.
//
// During the chain, `state->ip` and the offsets stored in `state->stack`
// may be stale; use `ip` and `cursor` for the current positions.
//
// Args:
//   data: Pointer to instruction-specific data.
//   state: Execution state.
//   ip: Pointer to the instruction being executed.
//   cursor: Current frame's `base` and live stack `top`.
//
// Returns:
//   True if execution should continue from the stored position.
//   False if execution stopped; `state->status` must indicate why.
typedef bool (*u7_vm_instruction_execute_fn_t)(
    void* data, struct u7_vm_state* state, struct u7_vm_instruction const* ip,
    struct u7_vm_stack_frame_cursor cursor) U7_VM_INSTRUCTION_EXEC_CALL_CONV;

// Defines the interface to an instruction.
//
// This struct does not represent ownership. It may be copied, but the lifetime
// of `data` must be managed independently.
struct u7_vm_instruction {
  void* data;
  u7_vm_instruction_execute_fn_t execute_fn;
};

// Executes an instruction chain starting at `state->ip`.
// Requires `state->ip` and the offsets stored in `state->stack` to be
// up to date.
#define U7_VM_INSTRUCTION_EXECUTE(state)                            \
  ((state)->ip->execute_fn((state)->ip->data, (state), (state)->ip, \
                           u7_vm_stack_load_frame_cursor(&(state)->stack)))

// Defines an instruction's execution function. The body receives `self`,
// `state`, and `cursor`.
//
// Return true to chain execution to the next instruction, forwarding
// `cursor` unchanged. Return false to stop at the current instruction;
// `state->ip` is then set to that instruction, and the cursor's position
// is stored in `state->stack`. Before returning false, set `state->status`
// to indicate why execution stopped.
//
// The body must not modify `state->ip` or rely on it identifying the
// current instruction. It must not push or pop stack values or frames,
// or relocate the stack.
//
// For instructions that need their current position, control where
// execution continues or stops, change the cursor, or relocate the stack,
// use U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT below.
#define U7_VM_DEFINE_INSTRUCTION_EXEC(fn_name, self_type)                      \
  U7_VM_ATTRIBUTE_ALWAYS_INLINE static inline bool fn_name##_impl(             \
      self_type const* self, struct u7_vm_state* state,                        \
      struct u7_vm_stack_frame_cursor cursor);                                 \
                                                                               \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV static bool fn_name(                        \
      void* data, struct u7_vm_state* state,                                   \
      struct u7_vm_instruction const* ip,                                      \
      struct u7_vm_stack_frame_cursor cursor) {                                \
    if (!fn_name##_impl((self_type const*)data, state, cursor)) {              \
      state->ip = ip;                                                          \
      u7_vm_stack_store_frame_cursor(&state->stack, cursor);                   \
      return false;                                                            \
    }                                                                          \
    struct u7_vm_instruction const* const next_ip = ip + 1;                    \
    assert(next_ip < state->instructions + state->instructions_size);          \
    __attribute__((musttail)) return next_ip->execute_fn(next_ip->data, state, \
                                                         next_ip, cursor);     \
  }                                                                            \
                                                                               \
  U7_VM_ATTRIBUTE_ALWAYS_INLINE static inline bool fn_name##_impl(             \
      U7_VM_ATTRIBUTE_MAYBE_UNUSED self_type const* self,                      \
      U7_VM_ATTRIBUTE_MAYBE_UNUSED struct u7_vm_state* state,                  \
      U7_VM_ATTRIBUTE_MAYBE_UNUSED struct u7_vm_stack_frame_cursor cursor)

// Result of a U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT body.
struct u7_vm_instruction_exec_explicit_result {
  struct u7_vm_instruction const* /*nonnull*/ ip;  // Next or stopping position.
  struct u7_vm_stack_frame_cursor cursor;          // Live stack position.
  bool stop;
};

// Constructs a result for a U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT body.
// `.stop` defaults to false. `.ip` and `.cursor` are required for both
// continuation and stopping.
#define U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT(...) \
  ((struct u7_vm_instruction_exec_explicit_result){__VA_ARGS__})

// Defines an instruction's execution function. The body receives `self`,
// `state`, `ip`, and `cursor`, where `ip` points to the current instruction
// and `cursor` describes the live stack position.
//
// Unlike U7_VM_DEFINE_INSTRUCTION_EXEC, this macro lets the body choose
// where execution continues or stops, and change the cursor.
//
// Return U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT with `.ip` and `.cursor`
// specifying the next instruction and its stack position. Use
// `.ip = ip + 1, .cursor = cursor` to execute the following instruction
// with the same stack position.
//
// The returned cursor must describe the live position in the stack's
// current allocation, including any changes made by the body.
//
// Set `.stop = true` to stop the instruction chain. The returned `ip`
// and cursor's position are stored in `state`. Before stopping, set
// `state->status` to indicate why execution stopped. For example, yield
// returns `.ip = ip + 1, .cursor = cursor, .stop = true` to resume after
// itself.
//
// The body must not modify `state->ip` or rely on it identifying the
// current instruction.
#define U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(fn_name, self_type)         \
  U7_VM_ATTRIBUTE_ALWAYS_INLINE static inline struct                       \
      u7_vm_instruction_exec_explicit_result                               \
      fn_name##_impl(self_type const* self, struct u7_vm_state* state,     \
                     struct u7_vm_instruction const* ip,                   \
                     struct u7_vm_stack_frame_cursor cursor);              \
                                                                           \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV static bool fn_name(                    \
      void* data, struct u7_vm_state* state,                               \
      struct u7_vm_instruction const* ip,                                  \
      struct u7_vm_stack_frame_cursor cursor) {                            \
    struct u7_vm_instruction_exec_explicit_result const next =             \
        fn_name##_impl((self_type const*)data, state, ip, cursor);         \
    assert(next.ip != NULL);                                               \
    assert(next.ip >= state->instructions &&                               \
           next.ip < state->instructions + state->instructions_size);      \
    assert(next.cursor.base != NULL);                                      \
    if (U7_UNLIKELY(next.stop)) {                                          \
      state->ip = next.ip;                                                 \
      u7_vm_stack_store_frame_cursor(&state->stack, next.cursor);          \
      return false;                                                        \
    }                                                                      \
    __attribute__((musttail)) return next.ip->execute_fn(                  \
        next.ip->data, state, next.ip, next.cursor);                       \
  }                                                                        \
                                                                           \
  U7_VM_ATTRIBUTE_ALWAYS_INLINE static inline struct                       \
      u7_vm_instruction_exec_explicit_result                               \
      fn_name##_impl(                                                      \
          U7_VM_ATTRIBUTE_MAYBE_UNUSED self_type const* self,              \
          U7_VM_ATTRIBUTE_MAYBE_UNUSED struct u7_vm_state* state,          \
          U7_VM_ATTRIBUTE_MAYBE_UNUSED struct u7_vm_instruction const* ip, \
          U7_VM_ATTRIBUTE_MAYBE_UNUSED struct u7_vm_stack_frame_cursor cursor)

// Defines an instruction's execution function with a separate cold failure
// path. The body receives `self`, `state`, and `cursor`.
//
// Return true to chain execution to the next instruction. Return false to
// tail-call the failure function, which sets `state->ip` to the current
// instruction and stores the cursor's position in `state->stack` before
// running the failure body. The chain stops when the failure body returns.
//
// The body must not modify `state->ip` or rely on it identifying the
// current instruction.
//
// `cursor` is forwarded unchanged to the next instruction or failure
// function. The body must not push or pop stack values or frames,
// or relocate the stack.
//
// Define the failure function with
// U7_VM_DEFINE_INSTRUCTION_FAILURE_FN(fn_name, self_type)
// before using this macro.
#define U7_VM_DEFINE_INSTRUCTION_EXEC_WITH_COLD_FAILURE(fn_name, self_type)    \
  U7_VM_ATTRIBUTE_ALWAYS_INLINE static inline bool fn_name##_impl(             \
      self_type const* self, struct u7_vm_state* state,                        \
      struct u7_vm_stack_frame_cursor cursor);                                 \
                                                                               \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV                                             \
  U7_VM_ATTRIBUTE_COLD U7_VM_ATTRIBUTE_NOINLINE static bool fn_name##_failure( \
      void* data, struct u7_vm_state* state,                                   \
      struct u7_vm_instruction const* ip,                                      \
      struct u7_vm_stack_frame_cursor cursor);                                 \
                                                                               \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV static bool fn_name(                        \
      void* data, struct u7_vm_state* state,                                   \
      struct u7_vm_instruction const* ip,                                      \
      struct u7_vm_stack_frame_cursor cursor) {                                \
    if (U7_UNLIKELY(!fn_name##_impl((self_type const*)data, state, cursor))) { \
      __attribute__((musttail)) return fn_name##_failure(data, state, ip,      \
                                                         cursor);              \
    }                                                                          \
    struct u7_vm_instruction const* const next_ip = ip + 1;                    \
    assert(next_ip < state->instructions + state->instructions_size);          \
    __attribute__((musttail)) return next_ip->execute_fn(next_ip->data, state, \
                                                         next_ip, cursor);     \
  }                                                                            \
                                                                               \
  U7_VM_ATTRIBUTE_ALWAYS_INLINE static inline bool fn_name##_impl(             \
      U7_VM_ATTRIBUTE_MAYBE_UNUSED self_type const* self,                      \
      U7_VM_ATTRIBUTE_MAYBE_UNUSED struct u7_vm_state* state,                  \
      U7_VM_ATTRIBUTE_MAYBE_UNUSED struct u7_vm_stack_frame_cursor cursor)

// Defines the cold failure function for an instruction defined with
// U7_VM_DEFINE_INSTRUCTION_EXEC_WITH_COLD_FAILURE.
//
// The body receives `self` and `state`. On entry, `state->ip` points to
// the current instruction, and the stack's stored offsets are up to date.
//
// The body returns void and must set `state->status` to indicate the
// failure. It may adjust `state->ip` if a different stopping position
// is required. It must leave the stack's stored offsets up to date.
// The instruction chain stops when the body returns.
#define U7_VM_DEFINE_INSTRUCTION_FAILURE_FN(fn_name, self_type)                \
  static inline void fn_name##_failure_impl(self_type const* self,             \
                                            struct u7_vm_state* state);        \
                                                                               \
  U7_VM_INSTRUCTION_EXEC_CALL_CONV                                             \
  U7_VM_ATTRIBUTE_COLD U7_VM_ATTRIBUTE_NOINLINE static bool fn_name##_failure( \
      void* data, struct u7_vm_state* state,                                   \
      struct u7_vm_instruction const* ip,                                      \
      struct u7_vm_stack_frame_cursor cursor) {                                \
    state->ip = ip;                                                            \
    u7_vm_stack_store_frame_cursor(&state->stack, cursor);                     \
    fn_name##_failure_impl((self_type const*)data, state);                     \
    return false;                                                              \
  }                                                                            \
                                                                               \
  static inline void fn_name##_failure_impl(                                   \
      U7_VM_ATTRIBUTE_MAYBE_UNUSED self_type const* self,                      \
      U7_VM_ATTRIBUTE_MAYBE_UNUSED struct u7_vm_state* state)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_INSTRUCTION_H_
