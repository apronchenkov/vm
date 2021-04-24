#include "@/public/vm0.h"

#include "@/public/stack_push_pop.h"
#include "@/public/state.h"

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

// u7_vm0_load_constant

#define U7_VM0_INSTRUCTION_0(name)                        \
  struct u7_vm0_instruction u7_vm0_##name() {             \
    struct u7_vm0_instruction result = {                  \
        .base = {.execute_fn = u7_vm0_##name##_execute}}; \
    return result;                                        \
  }

static bool u7_vm0_load_constant_i32_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_i32(&state->stack,
                       ((struct u7_vm0_instruction const*)self)->arg1.i32);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_load_constant_i64_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_i64(&state->stack,
                       ((struct u7_vm0_instruction const*)self)->arg1.i64);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_load_constant_f32_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_f32(&state->stack,
                       ((struct u7_vm0_instruction const*)self)->arg1.f32);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_load_constant_f64_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_f64(&state->stack,
                       ((struct u7_vm0_instruction const*)self)->arg1.f64);
  return u7_vm_instruction_tail_call(tail, self, state);
}

struct u7_vm0_instruction u7_vm0_load_constant_i32(int32_t value) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_constant_i32_execute},
      .arg1 = {.i32 = value},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_constant_i64(int64_t value) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_constant_i64_execute},
      .arg1 = {.i64 = value},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_constant_f32(float value) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_constant_f32_execute},
      .arg1 = {.f32 = value},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_constant_f64(double value) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_constant_f64_execute},
      .arg1 = {.f64 = value},
  };
  return result;
}

// load_local

static bool u7_vm0_load_local_i32_execute(int tail,
                                          struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(int32_t) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(int32_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  u7_vm_stack_push_i32(&state->stack,
                       *(int32_t*)u7_vm_memory_add_offset(
                           u7_vm_stack_current_locals(&state->stack), offset));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_load_local_i64_execute(int tail,
                                          struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(int64_t) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(int64_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  u7_vm_stack_push_i64(&state->stack,
                       *(int64_t*)u7_vm_memory_add_offset(
                           u7_vm_stack_current_locals(&state->stack), offset));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_load_local_f32_execute(int tail,
                                          struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(float) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(float) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  u7_vm_stack_push_f32(&state->stack,
                       *(float*)u7_vm_memory_add_offset(
                           u7_vm_stack_current_locals(&state->stack), offset));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_load_local_f64_execute(int tail,
                                          struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(double) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(double) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  u7_vm_stack_push_f32(&state->stack,
                       *(double*)u7_vm_memory_add_offset(
                           u7_vm_stack_current_locals(&state->stack), offset));
  return u7_vm_instruction_tail_call(tail, self, state);
}

struct u7_vm0_instruction u7_vm0_load_local_i32(
    struct u7_vm0_local_variable var) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_local_i32_execute},
      .arg1 = {.i64 = var.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_local_i64(
    struct u7_vm0_local_variable var) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_local_i64_execute},
      .arg1 = {.i64 = var.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_local_f32(
    struct u7_vm0_local_variable var) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_local_f32_execute},
      .arg1 = {.i64 = var.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_local_f64(
    struct u7_vm0_local_variable var) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_local_f64_execute},
      .arg1 = {.i64 = var.offset},
  };
  return result;
}

// store_local

static bool u7_vm0_store_local_i32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(int32_t) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(int32_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  *(int32_t*)u7_vm_memory_add_offset(u7_vm_stack_current_locals(&state->stack),
                                     offset) =
      u7_vm_stack_pop_i32(&state->stack);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_store_local_i64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(int64_t) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(int64_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  *(int64_t*)u7_vm_memory_add_offset(u7_vm_stack_current_locals(&state->stack),
                                     offset) =
      u7_vm_stack_pop_i64(&state->stack);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_store_local_f32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(float) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(float) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  *(float*)u7_vm_memory_add_offset(u7_vm_stack_current_locals(&state->stack),
                                   offset) = u7_vm_stack_pop_f32(&state->stack);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_store_local_f64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(double) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(double) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  *(double*)u7_vm_memory_add_offset(u7_vm_stack_current_locals(&state->stack),
                                    offset) =
      u7_vm_stack_pop_f64(&state->stack);
  return u7_vm_instruction_tail_call(tail, self, state);
}

struct u7_vm0_instruction u7_vm0_store_local_i32(
    struct u7_vm0_local_variable var) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_store_local_i32_execute},
      .arg1 = {.i64 = var.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_store_local_i64(
    struct u7_vm0_local_variable var) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_store_local_i64_execute},
      .arg1 = {.i64 = var.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_store_local_f32(
    struct u7_vm0_local_variable var) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_store_local_f32_execute},
      .arg1 = {.i64 = var.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_store_local_f64(
    struct u7_vm0_local_variable var) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_store_local_f64_execute},
      .arg1 = {.i64 = var.offset},
  };
  return result;
}

// compare

static bool u7_vm0_compare_i32_execute(int tail,
                                       struct u7_vm_instruction const* self,
                                       struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t lhs = u7_vm_stack_pop_i32(&state->stack);
  u7_vm_stack_push_i32(&state->stack, (lhs < rhs ? -1 : (lhs > rhs ? 1 : 0)));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_compare_i64_execute(int tail,
                                       struct u7_vm_instruction const* self,
                                       struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t lhs = u7_vm_stack_pop_i64(&state->stack);
  u7_vm_stack_push_i32(&state->stack, (lhs < rhs ? -1 : (lhs > rhs ? 1 : 0)));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_compare_f32_execute(int tail,
                                       struct u7_vm_instruction const* self,
                                       struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float lhs = u7_vm_stack_pop_f32(&state->stack);
  u7_vm_stack_push_i32(&state->stack, (lhs < rhs ? -1 : (lhs > rhs ? 1 : 0)));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_compare_f64_execute(int tail,
                                       struct u7_vm_instruction const* self,
                                       struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double lhs = u7_vm_stack_pop_f64(&state->stack);
  u7_vm_stack_push_i32(&state->stack, (lhs < rhs ? -1 : (lhs > rhs ? 1 : 0)));
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(compare_i32)
U7_VM0_INSTRUCTION_0(compare_i64)
U7_VM0_INSTRUCTION_0(compare_f32)
U7_VM0_INSTRUCTION_0(compare_f64)

// jump

static bool u7_vm0_jump_if_i32_zero_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) == 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i32_negative_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) < 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i32_positive_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) > 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i32_not_zero_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) != 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i32_negative_or_zero_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) <= 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i32_positive_or_zero_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) >= 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i64_zero_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i64(&state->stack) == 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i64_negative_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i64(&state->stack) < 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i64_positive_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i64(&state->stack) > 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i64_not_zero_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i64(&state->stack) != 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i64_negative_or_zero_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i64(&state->stack) <= 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_if_i64_positive_or_zero_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i64(&state->stack) >= 0) {
    state->ip += offset;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_jump_execute(int tail, struct u7_vm_instruction const* self,
                                struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  state->ip += offset;
  return u7_vm_instruction_tail_call(tail, self, state);
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_zero(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_zero_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_not_zero(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_not_zero_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_negative(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_negative_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_negative_or_zero(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_negative_or_zero_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_positive(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_positive_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_positive_or_zero(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_positive_or_zero_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i64_zero(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i64_zero_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i64_not_zero(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i64_not_zero_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i64_negative(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i64_negative_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i64_negative_or_zero(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i64_negative_or_zero_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i64_positive(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i64_positive_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i64_positive_or_zero(
    struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i64_positive_or_zero_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump(struct u7_vm0_local_label label) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_execute},
      .arg1 = {.i64 = label.offset},
  };
  return result;
}

// u7_vm0_operation

// duplicate

static bool u7_vm0_duplicate_i32_execute(int tail,
                                         struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_duplicate_i32(&state->stack);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_duplicate_i64_execute(int tail,
                                         struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_duplicate_i64(&state->stack);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_duplicate_f32_execute(int tail,
                                         struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_duplicate_f32(&state->stack);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_duplicate_f64_execute(int tail,
                                         struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_duplicate_f64(&state->stack);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(duplicate_i32)
U7_VM0_INSTRUCTION_0(duplicate_i64)
U7_VM0_INSTRUCTION_0(duplicate_f32)
U7_VM0_INSTRUCTION_0(duplicate_f64)

// not

static bool u7_vm0_bitwise_not_i32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  *p = ~(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_bitwise_not_i64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  *p = ~(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(bitwise_not_i32)
U7_VM0_INSTRUCTION_0(bitwise_not_i64)

// and

static bool u7_vm0_bitwise_and_i32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  *p &= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_bitwise_and_i64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  *p &= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(bitwise_and_i32)
U7_VM0_INSTRUCTION_0(bitwise_and_i64)

// or

static bool u7_vm0_bitwise_or_i32_execute(int tail,
                                          struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  *p |= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_bitwise_or_i64_execute(int tail,
                                          struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  *p |= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(bitwise_or_i32)
U7_VM0_INSTRUCTION_0(bitwise_or_i64)

// xor

static bool u7_vm0_bitwise_xor_i32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  *p |= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_bitwise_xor_i64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  *p |= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(bitwise_xor_i32)
U7_VM0_INSTRUCTION_0(bitwise_xor_i64)

// abs

static bool u7_vm0_abs_i32_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t x = *p;
  if (x == INT32_MIN) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_abs_i32 x=%" PRId32, x);
    return false;
  } else {
    *p = (x >= 0 ? x : -x);
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_abs_i64_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t x = *p;
  if (x == INT64_MIN) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_abs_i64 x=%" PRId64, x);
    return false;
  } else {
    *p = (x >= 0 ? x : -x);
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_abs_f32_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float* x = u7_vm_stack_peek_f32(&state->stack);
  *x = __builtin_fabsf(*x);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_abs_f64_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double* x = u7_vm_stack_peek_f64(&state->stack);
  *x = __builtin_fabs(*x);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(abs_i32)
U7_VM0_INSTRUCTION_0(abs_i64)
U7_VM0_INSTRUCTION_0(abs_f32)
U7_VM0_INSTRUCTION_0(abs_f64)

// neg

static bool u7_vm0_neg_i32_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t x = *p;
  if (x == INT32_MIN) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_neg_i32 x=%" PRId32, x);
    return false;
  } else {
    *p = -x;
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_neg_i64_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t x = *p;
  if (x == INT64_MIN) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_neg_i64 x=%" PRId64, x);
    return false;
  } else {
    *p = -x;
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_neg_f32_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float* p = u7_vm_stack_peek_f32(&state->stack);
  float x = *p;
  *p = -x;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_neg_f64_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double* p = u7_vm_stack_peek_f64(&state->stack);
  double x = *p;
  *p = -x;
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(neg_i32)
U7_VM0_INSTRUCTION_0(neg_i64)
U7_VM0_INSTRUCTION_0(neg_f32)
U7_VM0_INSTRUCTION_0(neg_f64)

// inc

static bool u7_vm0_inc_i32_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t x = *p;
  int32_t delta = ((struct u7_vm0_instruction const*)self)->arg1.i32;
  if (__builtin_add_overflow(x, delta, p)) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_inc_i32 x=%" PRId32
                             ", delta=%" PRId32,
                             x, delta);
    return false;
  } else {
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_inc_i64_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t x = *p;
  int64_t delta = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  if (__builtin_add_overflow(x, delta, p)) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_inc_i64 x=%" PRId64
                             ", delta=%" PRId64,
                             x, delta);
    return false;
  } else {
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_inc_f32_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs += ((struct u7_vm0_instruction const*)self)->arg1.f32;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_inc_f64_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs += ((struct u7_vm0_instruction const*)self)->arg1.f64;
  return u7_vm_instruction_tail_call(tail, self, state);
}

struct u7_vm0_instruction u7_vm0_inc_i32(int32_t delta) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_inc_i32_execute},
      .arg1 = {.i32 = delta},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_inc_i64(int64_t delta) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_inc_i64_execute},
      .arg1 = {.i64 = delta},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_inc_f32(float delta) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_inc_f32_execute},
      .arg1 = {.f32 = delta},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_inc_f64(double delta) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_inc_f64_execute},
      .arg1 = {.f64 = delta},
  };
  return result;
}

// inc_local

static bool u7_vm0_inc_local_i64_execute(int tail,
                                         struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  int64_t delta = ((struct u7_vm0_instruction const*)self)->arg2.i64;
  assert(offset % _Alignof(int64_t) == 0);
  assert(offset >= 0);
  assert(offset + sizeof(int64_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  int64_t* p = (int64_t*)u7_vm_memory_add_offset(
      u7_vm_stack_current_locals(&state->stack), offset);
  int64_t x = *p;
  if (__builtin_add_overflow(x, delta, p)) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_inc_local_i64 x=%" PRId64
                             ", delta=%" PRId64,
                             x, delta);
    return false;
  }
  u7_vm_stack_push_i32(&state->stack, *p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

struct u7_vm0_instruction u7_vm0_inc_local_i64(struct u7_vm0_local_variable var,
                                               int64_t delta) {
  assert(var.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_inc_local_i64_execute},
      .arg1 = {.i64 = var.offset},
      .arg2 = {.i64 = delta},
  };
  return result;
}

// add

static bool u7_vm0_add_i32_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);

  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t lhs = *p;
  if (__builtin_add_overflow(lhs, rhs, p)) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_add_i32 lhs=%" PRId32
                             ", rhs=%" PRId32,
                             lhs, rhs);
    return false;
  }

  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_add_i64_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t lhs = *p;
  if (__builtin_add_overflow(lhs, rhs, p)) {
    state->error = u7_errorf(u7_errno_category(), ERANGE,
                             "integer overflow: u7_vm0_add_i64 lhs=%" PRId64
                             ", rhs=%" PRId64,
                             lhs, rhs);
    return false;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_add_f32_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs += rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_add_f64_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs += rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(add_i32)
U7_VM0_INSTRUCTION_0(add_i64)
U7_VM0_INSTRUCTION_0(add_f32)
U7_VM0_INSTRUCTION_0(add_f64)

// subtract

static bool u7_vm0_subtract_i32_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t lhs = *p;
  if (__builtin_sub_overflow(lhs, rhs, p)) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_subtract_i32 lhs=%" PRId32 ", rhs=%" PRId32,
        lhs, rhs);
    return false;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_subtract_i64_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t lhs = *p;
  if (__builtin_sub_overflow(lhs, rhs, p)) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_subtract_i64 lhs=%" PRId64 ", rhs=%" PRId64,
        lhs, rhs);
    return false;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_subtract_f32_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs -= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_subtract_f64_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs -= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(subtract_i32)
U7_VM0_INSTRUCTION_0(subtract_i64)
U7_VM0_INSTRUCTION_0(subtract_f32)
U7_VM0_INSTRUCTION_0(subtract_f64)

// multiply

static bool u7_vm0_multiply_i32_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t lhs = *p;
  if (__builtin_mul_overflow(lhs, rhs, p)) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_multiply_i32 lhs=%" PRId32 ", rhs=%" PRId32,
        lhs, rhs);
    return false;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_multiply_i64_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t lhs = *p;
  if (__builtin_mul_overflow(lhs, rhs, p)) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_multiply_i64 lhs=%" PRId64 ", rhs=%" PRId64,
        lhs, rhs);
    return false;
  }
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_multiply_f32_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs *= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_multiply_f64_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs *= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(multiply_i32)
U7_VM0_INSTRUCTION_0(multiply_i64)
U7_VM0_INSTRUCTION_0(multiply_f32)
U7_VM0_INSTRUCTION_0(multiply_f64)

// divide

static bool u7_vm0_divide_f32_execute(int tail,
                                      struct u7_vm_instruction const* self,
                                      struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs /= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_divide_f64_execute(int tail,
                                      struct u7_vm_instruction const* self,
                                      struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs /= rhs;
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(divide_f32)
U7_VM0_INSTRUCTION_0(divide_f64)

// floordiv

static bool u7_vm0_floordiv_u32_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  uint32_t rhs = (uint32_t)u7_vm_stack_pop_i32(&state->stack);
  uint32_t* lhs = (uint32_t*)u7_vm_stack_peek_i32(&state->stack);
  if (rhs == 0) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "division by zero: u7_vm0_floordiv_u32 lhs=%" PRIu32 ", rhs=%" PRIu32,
        *lhs, rhs);
    return false;
  } else {
    *lhs /= rhs;
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_floordiv_u64_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);

  uint64_t rhs = (uint64_t)u7_vm_stack_pop_i64(&state->stack);
  uint64_t* lhs = (uint64_t*)u7_vm_stack_peek_i64(&state->stack);
  if (rhs == 0) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "division by zero: u7_vm0_floordiv_u64 lhs=%" PRIu64 ", rhs=%" PRIu64,
        *lhs, rhs);
    return false;
  } else {
    *lhs /= rhs;
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

U7_VM0_INSTRUCTION_0(floordiv_u32)
U7_VM0_INSTRUCTION_0(floordiv_u64)

// floormod

static bool u7_vm0_floormod_u32_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  uint32_t rhs = (uint32_t)u7_vm_stack_pop_i32(&state->stack);
  uint32_t* lhs = (uint32_t*)u7_vm_stack_peek_i32(&state->stack);
  if (rhs == 0) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "modision by zero: u7_vm0_floormod_u32 lhs=%" PRIu32 ", rhs=%" PRIu32,
        *lhs, rhs);
    return false;
  } else {
    *lhs %= rhs;
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_floormod_u64_execute(int tail,
                                        struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  uint64_t rhs = (uint64_t)u7_vm_stack_pop_i64(&state->stack);
  uint64_t* lhs = (uint64_t*)u7_vm_stack_peek_i64(&state->stack);
  if (rhs == 0) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "modision by zero: u7_vm0_floormod_u64 lhs=%" PRIu64 ", rhs=%" PRIu64,
        *lhs, rhs);
    return false;
  } else {
    *lhs %= rhs;
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

U7_VM0_INSTRUCTION_0(floormod_u32)
U7_VM0_INSTRUCTION_0(floormod_u64)

// floormod_local

static bool u7_vm0_floormod_local_u64_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);

  int64_t lhs_offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(lhs_offset % _Alignof(int64_t) == 0);
  assert(lhs_offset >= 0);
  assert(lhs_offset + sizeof(int64_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  uint64_t lhs = *(int64_t*)u7_vm_memory_add_offset(
      u7_vm_stack_current_locals(&state->stack), lhs_offset);
  int64_t rhs_offset = ((struct u7_vm0_instruction const*)self)->arg2.i64;
  assert(rhs_offset % _Alignof(int64_t) == 0);
  assert(rhs_offset >= 0);
  assert(rhs_offset + sizeof(int64_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  uint64_t rhs = *(int64_t*)u7_vm_memory_add_offset(
      u7_vm_stack_current_locals(&state->stack), rhs_offset);
  if (rhs == 0) {
    state->error = u7_errorf(
        u7_errno_category(), ERANGE,
        "modision by zero: u7_vm0_floormod_u64 lhs=%" PRIu64 ", rhs=%" PRIu64,
        lhs, rhs);
    return false;
  } else {
    u7_vm_stack_push_i64(&state->stack, (int64_t)(lhs % rhs));
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

struct u7_vm0_instruction u7_vm0_floormod_local_u64(
    struct u7_vm0_local_variable lhs, struct u7_vm0_local_variable rhs) {
  assert(lhs.offset >= 0);
  assert(rhs.offset >= 0);
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_floormod_local_u64_execute},
      .arg1 = {.i64 = lhs.offset},
      .arg2 = {.i64 = rhs.offset},
  };
  return result;
}

// rounding

static bool u7_vm0_floor_f32_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_floorf(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_floor_f64_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_floor(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(floor_f32)
U7_VM0_INSTRUCTION_0(floor_f64)

static bool u7_vm0_ceil_f32_execute(int tail,
                                    struct u7_vm_instruction const* self,
                                    struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_ceilf(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_ceil_f64_execute(int tail,
                                    struct u7_vm_instruction const* self,
                                    struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_ceil(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(ceil_f32)
U7_VM0_INSTRUCTION_0(ceil_f64)

static bool u7_vm0_round_f32_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_roundf(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_round_f64_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_round(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(round_f32)
U7_VM0_INSTRUCTION_0(round_f64)

static bool u7_vm0_trunc_f32_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_truncf(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_trunc_f64_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_trunc(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(trunc_f32)
U7_VM0_INSTRUCTION_0(trunc_f64)

// sqrt

static bool u7_vm0_sqrt_f32_execute(int tail,
                                    struct u7_vm_instruction const* self,
                                    struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_sqrtf(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_sqrt_f64_execute(int tail,
                                    struct u7_vm_instruction const* self,
                                    struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_sqrt(*p);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(sqrt_f32)
U7_VM0_INSTRUCTION_0(sqrt_f64)

// cast

static bool u7_vm0_cast_i32_to_i64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_i64(&state->stack, u7_vm_stack_pop_i32(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_cast_i32_to_f32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_f32(&state->stack, u7_vm_stack_pop_i32(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_cast_i32_to_f64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_f64(&state->stack, u7_vm_stack_pop_i32(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_cast_i64_to_i32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  int64_t x = u7_vm_stack_pop_i64(&state->stack);
  if (x < INT32_MIN || x > INT32_MAX) {
    state->error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_cast_i64_to_i32 x=%" PRId64, x);
    return false;
  } else {
    u7_vm_stack_push_i32(&state->stack, x);
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_cast_i64_to_f32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_f32(&state->stack, u7_vm_stack_pop_i64(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_cast_i64_to_f64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_f64(&state->stack, u7_vm_stack_pop_i64(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_cast_f32_to_f64_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_f64(&state->stack, u7_vm_stack_pop_f32(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_cast_f64_to_f32_execute(int tail,
                                           struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  u7_vm_stack_push_f32(&state->stack, u7_vm_stack_pop_f64(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(cast_i32_to_i64)
U7_VM0_INSTRUCTION_0(cast_i32_to_f32)
U7_VM0_INSTRUCTION_0(cast_i32_to_f64)
U7_VM0_INSTRUCTION_0(cast_i64_to_i32)
U7_VM0_INSTRUCTION_0(cast_i64_to_f32)
U7_VM0_INSTRUCTION_0(cast_i64_to_f64)
U7_VM0_INSTRUCTION_0(cast_f32_to_f64)
U7_VM0_INSTRUCTION_0(cast_f64_to_f32)

// trunc_f_to_i

static bool u7_vm0_trunc_f32_to_i32_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  float x = u7_vm_stack_pop_f32(&state->stack);
  // FIXME(apronchenkov): Make an check exact.
  if (x < (float)INT32_MIN || x > (float)INT32_MAX) {
    state->error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_trunc_f32_to_i32 x=%f", x);
    return false;
  } else {
    u7_vm_stack_push_i32(&state->stack, x);
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_trunc_f32_to_i64_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);

  float x = u7_vm_stack_pop_f32(&state->stack);
  // FIXME(apronchenkov): Make an check exact.
  if (x < (float)INT64_MIN || x > (float)INT64_MAX) {
    state->error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_trunc_f32_to_i64 x=%f", x);
    return false;
  } else {
    u7_vm_stack_push_i64(&state->stack, x);
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_trunc_f64_to_i32_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double x = u7_vm_stack_pop_f64(&state->stack);
  // FIXME(apronchenkov): Make an check exact.
  if (x < (double)INT32_MIN || x > (double)INT32_MAX) {
    state->error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_trunc_f64_to_i32 x=%f", x);
    return false;
  } else {
    u7_vm_stack_push_i32(&state->stack, x);
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

static bool u7_vm0_trunc_f64_to_i64_execute(
    int tail, struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  double x = u7_vm_stack_pop_f64(&state->stack);
  // FIXME(apronchenkov): Make an check exact.
  if (x < (double)INT64_MIN || x > (double)INT64_MAX) {
    state->error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_trunc_f64_to_i64 x=%f", x);
    return false;
  } else {
    u7_vm_stack_push_i64(&state->stack, x);
    return u7_vm_instruction_tail_call(tail, self, state);
  }
}

U7_VM0_INSTRUCTION_0(trunc_f32_to_i32)
U7_VM0_INSTRUCTION_0(trunc_f32_to_i64)
U7_VM0_INSTRUCTION_0(trunc_f64_to_i32)
U7_VM0_INSTRUCTION_0(trunc_f64_to_i64)

// print

static bool u7_vm0_print_i32_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  printf("%" PRId32, u7_vm_stack_pop_i32(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_print_i64_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  printf("%" PRId64, u7_vm_stack_pop_i64(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_print_f32_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  printf("%f", u7_vm_stack_pop_f32(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_print_f64_execute(int tail,
                                     struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  printf("%f", u7_vm_stack_pop_f64(&state->stack));
  return u7_vm_instruction_tail_call(tail, self, state);
}

static bool u7_vm0_println_execute(int tail,
                                   struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  printf("\n");
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(print_i32)
U7_VM0_INSTRUCTION_0(print_i64)
U7_VM0_INSTRUCTION_0(print_f32)
U7_VM0_INSTRUCTION_0(print_f64)
U7_VM0_INSTRUCTION_0(println)

// yield

static bool u7_vm0_yield_execute(int tail, struct u7_vm_instruction const* self,
                                 struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  return false;
}

U7_VM0_INSTRUCTION_0(yield)

// dump_state

static bool u7_vm0_dump_state_execute(int tail,
                                      struct u7_vm_instruction const* self,
                                      struct u7_vm_state* state) {
  u7_vm_instruction_enter(tail, self, state);
  printf("ip=%zd  bp=%zd  sp=%zd\n", state->ip, state->stack.base_offset,
         state->stack.top_offset);
  return u7_vm_instruction_tail_call(tail, self, state);
}

U7_VM0_INSTRUCTION_0(dump_state)
