#include "@/public/state.h"

#include "@/public/instruction.h"

#include <github.com/apronchenkov/u7_init/public/testing.h>
#include <stdbool.h>
#include <string.h>

static int g_step_count = 0;

U7_VM_DEFINE_INSTRUCTION_EXEC(execute_step, void) {
  g_step_count += 1;
  return true;
}

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_stop, void) {
  state->status = U7_VM_STATE_STATUS_HALTED;
  state->ip = ip;
  return U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT(.stop = true);
}

U7_TEST(test_dispatch_chains_musttail_and_stops) {
  struct u7_vm_instruction instructions[] = {
      {.execute_fn = execute_step},
      {.execute_fn = execute_step},
      {.execute_fn = execute_stop},
  };

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state_options options = u7_vm_state_options_default();
  options.statics_layout = &statics_layout;
  options.instructions = instructions;
  options.instructions_size = 3;
  struct u7_vm_state state;
  g_step_count = 0;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);

  U7_ASSERT_EQ(g_step_count, 2);
  U7_ASSERT(state.ip == state.instructions + 2);
  u7_vm_state_destroy(&state);
}

struct jump_instruction {
  size_t target;
};

static int g_jump_count = 0;

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_jump, struct jump_instruction) {
  g_jump_count += 1;
  return U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT(
      .ip = state->instructions + self->target, .base = base);
}

U7_TEST(test_dispatch_explicit_instruction_can_set_ip_and_continue) {
  struct jump_instruction jump = {.target = 2};

  // Instructions:
  //   0: jump 2
  //   1: step_count += 1 // not executed
  //   2: stop
  struct u7_vm_instruction instructions[] = {
      {.data = &jump, .execute_fn = execute_jump},
      {.execute_fn = execute_step},
      {.execute_fn = execute_stop},
  };

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state_options options = u7_vm_state_options_default();
  options.statics_layout = &statics_layout;
  options.instructions = instructions;
  options.instructions_size = 3;
  struct u7_vm_state state;
  g_step_count = 0;
  g_jump_count = 0;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);

  U7_ASSERT_EQ(g_jump_count, 1);
  U7_ASSERT_EQ(g_step_count, 0);
  U7_ASSERT(state.ip == state.instructions + 2);
  u7_vm_state_destroy(&state);
}

// Regression test for musttail dispatch. Each step allocates a large local
// array and forces it to be materialized. With tail-call dispatch, the
// current stack frame is released before the next step runs, so native stack
// usage remains constant. A regular call would accumulate these frames and
// overflow the native stack after enough steps.
enum { kFatStepCount = 4096 };
enum { kFatStepPaddingSize = 1 << 16 };

static int g_fat_step_count = 0;
static volatile char g_fat_step_sink;

U7_VM_DEFINE_INSTRUCTION_EXEC(execute_fat_step, void) {
  char padding[kFatStepPaddingSize];
  memset(padding, 1, sizeof(padding));
  g_fat_step_sink = padding[sizeof(padding) - 1];
  g_fat_step_count += 1;
  return true;
}

U7_TEST(test_dispatch_does_not_grow_the_native_stack) {
  static struct u7_vm_instruction instructions[kFatStepCount + 1];
  for (int i = 0; i < kFatStepCount; ++i) {
    instructions[i] =
        (struct u7_vm_instruction){.execute_fn = execute_fat_step};
  }
  instructions[kFatStepCount] =
      (struct u7_vm_instruction){.execute_fn = execute_stop};

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state_options options = u7_vm_state_options_default();
  options.statics_layout = &statics_layout;
  options.instructions = instructions;
  options.instructions_size = kFatStepCount + 1;
  struct u7_vm_state state;
  g_fat_step_count = 0;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);

  U7_ASSERT_EQ(g_fat_step_count, kFatStepCount);
  U7_ASSERT(state.ip == state.instructions + kFatStepCount);
  u7_vm_state_destroy(&state);
}

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_suspend, void) {
  state->ip = ip + 1;
  state->status = U7_VM_STATE_STATUS_SUSPENDED;
  return U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT(.stop = true);
}

U7_TEST(test_state_run_suspends_and_resumes_at_the_next_instruction) {
  struct u7_vm_instruction instructions[] = {
      {.execute_fn = execute_step},
      {.execute_fn = execute_suspend},
      {.execute_fn = execute_step},
      {.execute_fn = execute_stop},
  };

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state_options options = u7_vm_state_options_default();
  options.statics_layout = &statics_layout;
  options.instructions = instructions;
  options.instructions_size = 4;
  struct u7_vm_state state;
  g_step_count = 0;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_SUSPENDED);
  U7_ASSERT_EQ(g_step_count, 1);
  U7_ASSERT(state.ip == state.instructions + 2);

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);
  U7_ASSERT_EQ(g_step_count, 2);

  u7_vm_state_destroy(&state);
}

U7_TEST(test_state_run_on_a_halted_state_is_a_noop) {
  struct u7_vm_instruction instructions[] = {{.execute_fn = execute_stop}};

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state_options options = u7_vm_state_options_default();
  options.statics_layout = &statics_layout;
  options.instructions = instructions;
  options.instructions_size = 1;
  struct u7_vm_state state;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);
  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);

  u7_vm_state_destroy(&state);
}

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_corrupt, void) {
  state->status = U7_VM_STATE_STATUS_CORRUPTED;
  state->ip = ip;
  return U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT(.stop = true);
}

U7_TEST(test_state_run_on_a_corrupted_state_is_a_noop) {
  struct u7_vm_instruction instructions[] = {{.execute_fn = execute_corrupt}};

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state_options options = u7_vm_state_options_default();
  options.statics_layout = &statics_layout;
  options.instructions = instructions;
  options.instructions_size = 1;
  struct u7_vm_state state;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_CORRUPTED);
  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_CORRUPTED);

  u7_vm_state_destroy(&state);
}

// A stand-in for a "call"-like instruction: pushes a frame using the core
// primitive directly and continues. The VM itself has no notion of calls;
// pushing a frame is enough to make exception unwinding meaningful.
struct push_frame_instruction {
  struct u7_vm_stack_frame_layout const* layout;
};

// EXEC_EXPLICIT, not plain EXEC, because this body needs `ip` -- its own
// position -- while continuing, which plain EXEC's impl no longer receives.
U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_push_frame,
                                       struct push_frame_instruction) {
  U7_ASSERT_OK(u7_vm_stack_push_frame(&state->stack, self->layout, ip));
  return U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT(
      .ip = ip + 1, .base = u7_vm_stack_frame_base(&state->stack));
}

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_raise, void) {
  state->status = U7_VM_STATE_STATUS_EXCEPTION;
  state->ip = ip;
  return U7_VM_INSTRUCTION_EXEC_EXPLICIT_RESULT(.stop = true);
}

static int g_deinit_count = 0;

static void deinit_frame(struct u7_vm_stack_frame_layout const* layout,
                         void* memory) {
  (void)layout;
  (void)memory;
  g_deinit_count += 1;
}

static enum u7_vm_exception_handler_action recover_at_ip_2(
    struct u7_vm_state* state, void* data) {
  (void)data;
  state->ip = state->instructions + 2;
  return U7_VM_EXCEPTION_HANDLER_ACTION_RECOVER;
}

U7_TEST(test_state_exception_recovers_via_handler) {
  struct u7_vm_stack_frame_layout callee_layout = {
      .deinit_fn = deinit_frame,
      .exception_handler_fn = recover_at_ip_2,
      .description = "callee",
  };
  struct push_frame_instruction push_data = {.layout = &callee_layout};

  // Instructions:
  //   0: push_frame(callee)
  //   1: raise           (inside the callee; its handler recovers to 2)
  //   2: stop
  struct u7_vm_instruction instructions[] = {
      {.data = &push_data, .execute_fn = execute_push_frame},
      {.execute_fn = execute_raise},
      {.execute_fn = execute_stop},
  };

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state_options options = u7_vm_state_options_default();
  options.statics_layout = &statics_layout;
  options.instructions = instructions;
  options.instructions_size = 3;
  struct u7_vm_state state;
  g_deinit_count = 0;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);

  // RECOVER keeps the callee frame alive; it's the frame that stops HALTED.
  U7_ASSERT(state.stack.base_offset != 0);
  U7_ASSERT_EQ(g_deinit_count, 0);

  u7_vm_state_destroy(&state);
  U7_ASSERT_EQ(g_deinit_count, 1);  // destroy tears down whatever is left
}

static enum u7_vm_exception_handler_action decline(struct u7_vm_state* state,
                                                   void* data) {
  (void)state;
  (void)data;
  return U7_VM_EXCEPTION_HANDLER_ACTION_UNWIND;
}

U7_TEST(test_state_exception_unwinds_to_the_root_when_unhandled) {
  struct u7_vm_stack_frame_layout callee_layout = {
      .deinit_fn = deinit_frame,
      .exception_handler_fn = decline,
      .description = "callee",
  };
  struct push_frame_instruction push_data = {.layout = &callee_layout};

  // Instructions:
  //   0: push_frame(callee)
  //   1: raise           (inside the callee; its handler declines)
  struct u7_vm_instruction instructions[] = {
      {.data = &push_data, .execute_fn = execute_push_frame},
      {.execute_fn = execute_raise},
  };

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state_options options = u7_vm_state_options_default();
  options.statics_layout = &statics_layout;
  options.instructions = instructions;
  options.instructions_size = 2;
  struct u7_vm_state state;
  g_deinit_count = 0;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_EXCEPTION);
  U7_ASSERT_EQ(g_deinit_count, 1);           // the callee frame was unwound
  U7_ASSERT_EQ(state.stack.base_offset, 0);  // only the root frame remains
  // Repositioned to the root's own suspended point (the push_frame
  // instruction), not left at the original raise site inside the callee.
  U7_ASSERT(state.ip == state.instructions);

  // An unhandled exception is terminal, same as HALTED/CORRUPTED.
  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_EXCEPTION);

  u7_vm_state_destroy(&state);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
