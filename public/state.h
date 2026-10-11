#ifndef U7_VM_STATE_H_
#define U7_VM_STATE_H_

#include "@/public/instruction.h"
#include "@/public/stack.h"

#include <github.com/apronchenkov/u7_init/public/init.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Options controlling how a state is provisioned.
struct u7_vm_state_options {
  struct u7_vm_stack_frame_layout const* statics_layout;
  struct u7_vm_instruction const* instructions;
  size_t instructions_size;
  size_t initial_stack_capacity;
  struct u7_vm_allocator allocator;
};

// Returns default options: a modest initial stack capacity backed by
// u7_vm_default_allocator. statics_layout/instructions/instructions_size are
// left unset -- the caller must still fill those in.
struct u7_vm_state_options u7_vm_state_options_default();

enum u7_vm_state_status {
  // Initialized, not yet run.
  U7_VM_STATE_STATUS_READY,

  // Execution is in progress within u7_vm_state_run or u7_vm_state_resume.
  U7_VM_STATE_STATUS_RUNNING,

  // An instruction stopped the chain for an ordinary, expected pause with
  // no program-level error. u7_vm_state_resume resumes it where it left off.
  U7_VM_STATE_STATUS_SUSPENDED,

  // An exception was raised. `state->ip` identifies the instruction
  // associated with the exception in the current frame.
  U7_VM_STATE_STATUS_EXCEPTION,

  // Execution completed normally. The state cannot be resumed.
  U7_VM_STATE_STATUS_HALTED,

  // An instruction reported an unrecoverable failure. Its meaning is
  // defined by the instruction set; the core VM never sets this status.
  // The state cannot be resumed.
  U7_VM_STATE_STATUS_CORRUPTED,
};

struct u7_vm_state {
  struct u7_vm_instruction const* /*nonnull*/ instructions;
  size_t instructions_size;
  struct u7_vm_instruction const* /*nonnull*/ ip;
  enum u7_vm_state_status status;
  struct u7_vm_stack stack;
};

u7_error u7_vm_state_init(struct u7_vm_state* self,
                          struct u7_vm_state_options options);

void u7_vm_state_destroy(struct u7_vm_state* self);

// Runs a READY state. Returns the status reached when this call stops
// execution.
enum u7_vm_state_status u7_vm_state_run(struct u7_vm_state* self);

// Resumes a SUSPENDED state. Returns the status reached when this call stops
// execution.
enum u7_vm_state_status u7_vm_state_resume(struct u7_vm_state* self);

// Resumes a SUSPENDED state by raising an exception. Returns the status reached
// when this call stops execution.
enum u7_vm_state_status u7_vm_state_resume_with_exception(
    struct u7_vm_state* self);

static inline void* u7_vm_state_globals(struct u7_vm_state* self) {
  return u7_vm_stack_globals(&self->stack);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STATE_H_
