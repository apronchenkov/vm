#include <github.com/apronchenkov/error/public/error.h>
#include <github.com/apronchenkov/vm/public/state.h>
#include <github.com/apronchenkov/vm/public/vm0.h>
#include <stdio.h>

u7_error Main() {
  struct statics {
    char _;
  };
  struct u7_vm_stack_frame_layout statics_layout = {
      .locals_size =
          u7_vm_align_size(sizeof(struct statics), kU7VmDefaultAlignment),
      .extra_capacity = 64,
      .description = "statics",
  };
  struct u7_vm_state state;

  struct u7_vm0_instruction is[] = {
      u7_vm0_load_constant_i64(34),
      u7_vm0_load_constant_i64(17),
      u7_vm0_add_i64(),
      u7_vm0_print_i64(),
      u7_vm0_println(),
      u7_vm0_yield(),
  };

  struct u7_vm_instruction const* js[sizeof(is) / sizeof(is[0])];
  for (size_t i = 0; i < sizeof(is) / sizeof(is[0]); ++i) {
    js[i] = &is[i].base;
  }

  U7_RETURN_IF_ERROR(u7_vm_state_init(&state, u7_vm_default_allocator(),
                                      &statics_layout, &js[0],
                                      sizeof(js) / sizeof(js[0])));
  u7_vm_state_run(&state);
  u7_error result = u7_error_acquire(state.last_error);
  u7_vm_state_destroy(&state);
  return result;
}

int main() {
  u7_error error = Main();
  if (error.error_code) {
    fprintf(stderr, "Main: %" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
    u7_error_release(error);
    return -1;
  }
  return 0;
}
