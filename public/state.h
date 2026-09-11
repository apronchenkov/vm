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
  // Only observed while u7_vm_state_run is on the stack; never a return value
  // and never seen by a caller inspecting an idle state.
  U7_VM_STATE_STATUS_RUNNING,
  // An instruction stopped the chain for an ordinary, expected pause with no
  // program-level error. u7_vm_state_run resumes it where it left off.
  U7_VM_STATE_STATUS_SUSPENDED,
  // An instruction raised an exception. `state->ip` identifies the instruction
  // associated with the exception in the current frame.
  U7_VM_STATE_STATUS_EXCEPTION,
  // An instruction stopped the chain for normal, permanent completion.
  // Unlike SUSPENDED, calling u7_vm_state_run again is a no-op that returns
  // HALTED without executing anything.
  U7_VM_STATE_STATUS_HALTED,
  // Available for an instruction to indicate an unrecoverable breakage; the
  // core VM never sets this itself, and doesn't attach any particular cause
  // to it -- that's up to whatever application-level contract the
  // instruction set wants to build on top. Treated the same as HALTED:
  // permanent, and a no-op on the next u7_vm_state_run call.
  U7_VM_STATE_STATUS_CORRUPTED,
};

struct u7_vm_state {
  struct u7_vm_instruction const* instructions;
  size_t instructions_size;
  size_t ip;
  enum u7_vm_state_status status;
  struct u7_vm_stack stack;
};

u7_error u7_vm_state_init(struct u7_vm_state* self,
                          struct u7_vm_state_options options);

void u7_vm_state_destroy(struct u7_vm_state* self);

// Runs a ready state or resumes a suspended state.
//
// Returns the reason execution stopped. Calling this on a terminal state
// (HALTED, EXCEPTION, or CORRUPTED) returns its current status without
// executing anything.
enum u7_vm_state_status u7_vm_state_run(struct u7_vm_state* self);

static inline void* u7_vm_state_globals(struct u7_vm_state* self) {
  return u7_vm_stack_globals(&self->stack);
}

static inline void* u7_vm_state_locals(struct u7_vm_state* self) {
  return u7_vm_stack_locals(&self->stack);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STATE_H_
