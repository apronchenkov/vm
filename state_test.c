#include "@/public/state.h"

#include "@/public/instruction.h"

#include <github.com/apronchenkov/u7_init/public/testing.h>
#include <stdbool.h>
#include <string.h>

static int g_step_count = 0;

U7_VM_DEFINE_INSTRUCTION_EXEC(execute_step, void) {
  (void)self;
  (void)state;
  g_step_count += 1;
  return true;
}

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_stop, void) {
  (void)self;
  state->status = U7_VM_STATE_STATUS_HALTED;
  return false;
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

  U7_ASSERT(g_step_count == 2);
  U7_ASSERT(state.ip == 2);
  u7_vm_state_destroy(&state);
}

struct jump_instruction {
  size_t target;
};

static int g_jump_count = 0;

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_jump, struct jump_instruction) {
  g_jump_count += 1;
  state->ip = self->target;
  return true;
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

  U7_ASSERT(g_jump_count == 1);
  U7_ASSERT(g_step_count == 0);
  U7_ASSERT(state.ip == 2);
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
  (void)self;
  (void)state;
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

  U7_ASSERT(g_fat_step_count == kFatStepCount);
  U7_ASSERT(state.ip == kFatStepCount);
  u7_vm_state_destroy(&state);
}

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_suspend, void) {
  (void)self;
  state->ip += 1;
  state->status = U7_VM_STATE_STATUS_SUSPENDED;
  return false;
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
  U7_ASSERT(g_step_count == 1);
  U7_ASSERT(state.ip == 2);

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);
  U7_ASSERT(g_step_count == 2);

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
  (void)self;
  state->status = U7_VM_STATE_STATUS_CORRUPTED;
  return false;
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

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
