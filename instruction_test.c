#include "@/public/instruction.h"

#include "@/public/state.h"

#include <github.com/apronchenkov/u7_init/public/testing.h>
#include <stdbool.h>

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

static bool g_guarded_should_succeed = true;
static int g_guarded_call_count = 0;
static int g_guarded_failure_count = 0;

U7_VM_DEFINE_INSTRUCTION_FAILURE_FN(execute_guarded, void) {
  (void)self;
  g_guarded_failure_count += 1;
  state->status = U7_VM_STATE_STATUS_CORRUPTED;
  return false;
}

U7_VM_DEFINE_INSTRUCTION_EXEC_WITH_COLD_FAILURE(execute_guarded, void) {
  (void)self;
  (void)state;
  g_guarded_call_count += 1;
  return g_guarded_should_succeed;
}

U7_TEST(test_cold_failure_instruction_advances_ip_and_continues_on_success) {
  struct u7_vm_instruction instructions[] = {
      {.execute_fn = execute_guarded},
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
  g_guarded_call_count = 0;
  g_guarded_failure_count = 0;
  g_guarded_should_succeed = true;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_HALTED);

  U7_ASSERT(g_guarded_call_count == 1);
  U7_ASSERT(g_guarded_failure_count == 0);
  U7_ASSERT(g_step_count == 1);
  U7_ASSERT(state.ip == 2);
  u7_vm_state_destroy(&state);
}

U7_TEST(test_cold_failure_instruction_leaves_ip_and_stops_on_failure) {
  struct u7_vm_instruction instructions[] = {
      {.execute_fn = execute_guarded},
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
  g_guarded_call_count = 0;
  g_guarded_failure_count = 0;
  g_guarded_should_succeed = false;
  U7_ASSERT_OK(u7_vm_state_init(&state, options));

  U7_ASSERT(u7_vm_state_run(&state) == U7_VM_STATE_STATUS_CORRUPTED);

  U7_ASSERT(g_guarded_call_count == 1);
  U7_ASSERT(g_guarded_failure_count == 1);
  U7_ASSERT(g_step_count == 0);  // never reached
  U7_ASSERT(state.ip == 0);      // left at the failing instruction
  u7_vm_state_destroy(&state);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
