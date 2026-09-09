#include "@/public/state.h"

#include "@/public/instruction.h"

#include <github.com/apronchenkov/u7_init/public/testing.h>
#include <stdbool.h>
#include <string.h>

static int g_step_count = 0;

U7_VM_DEFINE_INSTRUCTION_EXEC(execute_step, struct u7_vm_instruction) {
  (void)self;
  (void)state;
  g_step_count += 1;
  return true;
}

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_stop, struct u7_vm_instruction) {
  (void)self;
  (void)state;
  return false;
}

U7_TEST(test_dispatch_chains_musttail_and_stops) {
  struct u7_vm_instruction step = {.execute_fn = execute_step};
  struct u7_vm_instruction stop = {.execute_fn = execute_stop};
  struct u7_vm_instruction const* instructions[] = {&step, &step, &stop};

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state state;
  g_step_count = 0;
  U7_ASSERT(
      u7_vm_state_init(&state, &statics_layout, instructions, 3).error_code ==
      0);

  u7_vm_state_run(&state);

  U7_ASSERT(g_step_count == 2);
  U7_ASSERT(state.ip == 2);
  u7_vm_state_destroy(&state);
}

struct jump_instruction {
  struct u7_vm_instruction base;
  size_t target;
};

static int g_jump_count = 0;

U7_VM_DEFINE_INSTRUCTION_EXEC_EXPLICIT(execute_jump, struct jump_instruction) {
  g_jump_count += 1;
  state->ip = self->target;
  return true;
}

U7_TEST(test_dispatch_explicit_instruction_can_set_ip_and_continue) {
  struct jump_instruction jump = {.base = {.execute_fn = execute_jump},
                                  .target = 2};
  struct u7_vm_instruction step = {.execute_fn = execute_step};
  struct u7_vm_instruction stop = {.execute_fn = execute_stop};

  // Instructions:
  //   0: jump 2
  //   1: step_count += 1 // not executed
  //   2: stop
  struct u7_vm_instruction const* instructions[] = {&jump.base, &step, &stop};

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state state;
  g_step_count = 0;
  g_jump_count = 0;
  U7_ASSERT(
      u7_vm_state_init(&state, &statics_layout, instructions, 3).error_code ==
      0);

  u7_vm_state_run(&state);

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

U7_VM_DEFINE_INSTRUCTION_EXEC(execute_fat_step, struct u7_vm_instruction) {
  (void)self;
  (void)state;
  char padding[kFatStepPaddingSize];
  memset(padding, 1, sizeof(padding));
  g_fat_step_sink = padding[sizeof(padding) - 1];
  g_fat_step_count += 1;
  return true;
}

U7_TEST(test_dispatch_does_not_grow_the_native_stack) {
  struct u7_vm_instruction fat_step = {.execute_fn = execute_fat_step};
  struct u7_vm_instruction stop = {.execute_fn = execute_stop};
  static struct u7_vm_instruction const* instructions[kFatStepCount + 1];
  for (int i = 0; i < kFatStepCount; ++i) {
    instructions[i] = &fat_step;
  }
  instructions[kFatStepCount] = &stop;

  struct u7_vm_stack_frame_layout statics_layout = {.locals_size = 0};
  struct u7_vm_state state;
  g_fat_step_count = 0;
  U7_ASSERT(
      u7_vm_state_init(&state, &statics_layout, instructions, kFatStepCount + 1)
          .error_code == 0);

  u7_vm_state_run(&state);

  U7_ASSERT(g_fat_step_count == kFatStepCount);
  U7_ASSERT(state.ip == kFatStepCount);
  u7_vm_state_destroy(&state);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
