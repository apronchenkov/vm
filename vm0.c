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

static void u7_vm0_load_constant_i32_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_stack_push_i32(&state->stack,
                       ((struct u7_vm0_instruction const*)self)->arg1.i32);
}

static void u7_vm0_load_constant_i64_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_stack_push_i64(&state->stack,
                       ((struct u7_vm0_instruction const*)self)->arg1.i64);
}

static void u7_vm0_load_constant_f32_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_stack_push_f32(&state->stack,
                       ((struct u7_vm0_instruction const*)self)->arg1.f32);
}

static void u7_vm0_load_constant_f64_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  u7_vm_stack_push_f64(&state->stack,
                       ((struct u7_vm0_instruction const*)self)->arg1.f64);
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

static void u7_vm0_load_local_i32_execute(struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  size_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(int32_t) == 0);
  assert(offset + sizeof(int32_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  u7_vm_stack_push_i32(&state->stack,
                       *(int32_t*)u7_vm_memory_add_offset(
                           u7_vm_stack_current_locals(&state->stack), offset));
}

static void u7_vm0_load_local_i64_execute(struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  size_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(int64_t) == 0);
  assert(offset + sizeof(int64_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  u7_vm_stack_push_i64(&state->stack,
                       *(int64_t*)u7_vm_memory_add_offset(
                           u7_vm_stack_current_locals(&state->stack), offset));
}

static void u7_vm0_load_local_f32_execute(struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  size_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(float) == 0);
  assert(offset + sizeof(float) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  u7_vm_stack_push_f32(&state->stack,
                       *(float*)u7_vm_memory_add_offset(
                           u7_vm_stack_current_locals(&state->stack), offset));
}

static void u7_vm0_load_local_f64_execute(struct u7_vm_instruction const* self,
                                          struct u7_vm_state* state) {
  size_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(double) == 0);
  assert(offset + sizeof(double) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  u7_vm_stack_push_f32(&state->stack,
                       *(double*)u7_vm_memory_add_offset(
                           u7_vm_stack_current_locals(&state->stack), offset));
}

struct u7_vm0_instruction u7_vm0_load_local_i32(size_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_local_i32_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_local_i64(size_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_local_i64_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_local_f32(size_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_local_f32_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_load_local_f64(size_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_load_local_f64_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

// store_local

static void u7_vm0_store_local_i32_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  size_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(int32_t) == 0);
  assert(offset + sizeof(int32_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  *(int32_t*)u7_vm_memory_add_offset(u7_vm_stack_current_locals(&state->stack),
                                     offset) =
      u7_vm_stack_pop_i32(&state->stack);
}

static void u7_vm0_store_local_i64_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  size_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(int64_t) == 0);
  assert(offset + sizeof(int64_t) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  *(int64_t*)u7_vm_memory_add_offset(u7_vm_stack_current_locals(&state->stack),
                                     offset) =
      u7_vm_stack_pop_i64(&state->stack);
}

static void u7_vm0_store_local_f32_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  size_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(float) == 0);
  assert(offset + sizeof(float) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  *(float*)u7_vm_memory_add_offset(u7_vm_stack_current_locals(&state->stack),
                                   offset) = u7_vm_stack_pop_f32(&state->stack);
}

static void u7_vm0_store_local_f64_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  size_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert(offset % _Alignof(double) == 0);
  assert(offset + sizeof(double) <=
         u7_vm_stack_current_frame_layout(&state->stack)->locals_size);
  *(double*)u7_vm_memory_add_offset(u7_vm_stack_current_locals(&state->stack),
                                    offset) =
      u7_vm_stack_pop_f64(&state->stack);
}

struct u7_vm0_instruction u7_vm0_store_local_i32(size_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_store_local_i32_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_store_local_i64(size_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_store_local_i64_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_store_local_f32(size_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_store_local_f32_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_store_local_f64(size_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_store_local_f64_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

// compare

static void u7_vm0_compare_i32_execute(struct u7_vm_instruction const* self,
                                       struct u7_vm_state* state) {
  (void)self;
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t lhs = u7_vm_stack_pop_i32(&state->stack);
  u7_vm_stack_push_i32(&state->stack, (lhs < rhs ? -1 : (lhs > rhs ? 1 : 0)));
}

static void u7_vm0_compare_i64_execute(struct u7_vm_instruction const* self,
                                       struct u7_vm_state* state) {
  (void)self;
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t lhs = u7_vm_stack_pop_i64(&state->stack);
  u7_vm_stack_push_i32(&state->stack, (lhs < rhs ? -1 : (lhs > rhs ? 1 : 0)));
}

static void u7_vm0_compare_f32_execute(struct u7_vm_instruction const* self,
                                       struct u7_vm_state* state) {
  (void)self;
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float lhs = u7_vm_stack_pop_f32(&state->stack);
  u7_vm_stack_push_i32(&state->stack, (lhs < rhs ? -1 : (lhs > rhs ? 1 : 0)));
}

static void u7_vm0_compare_f64_execute(struct u7_vm_instruction const* self,
                                       struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double lhs = u7_vm_stack_pop_f64(&state->stack);
  u7_vm_stack_push_i32(&state->stack, (lhs < rhs ? -1 : (lhs > rhs ? 1 : 0)));
}

U7_VM0_INSTRUCTION_0(compare_i32)
U7_VM0_INSTRUCTION_0(compare_i64)
U7_VM0_INSTRUCTION_0(compare_f32)
U7_VM0_INSTRUCTION_0(compare_f64)

// jump

static void u7_vm0_jump_if_i32_zero_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) == 0) {
    state->ip += offset;
  }
}

static void u7_vm0_jump_if_i32_negative_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) < 0) {
    state->ip += offset;
  }
}

static void u7_vm0_jump_if_i32_positive_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) > 0) {
    state->ip += offset;
  }
}

static void u7_vm0_jump_if_i32_not_zero_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) != 0) {
    state->ip += offset;
  }
}

static void u7_vm0_jump_if_i32_negative_or_zero_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) <= 0) {
    state->ip += offset;
  }
}

static void u7_vm0_jump_if_i32_positive_or_zero_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  if (u7_vm_stack_pop_i32(&state->stack) >= 0) {
    state->ip += offset;
  }
}

static void u7_vm0_jump_execute(struct u7_vm_instruction const* self,
                                struct u7_vm_state* state) {
  int64_t offset = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  assert((offset >= 0 && state->ip + offset < state->instructions_size) ||
         (offset < 0 && state->ip >= -(size_t)offset));
  state->ip += offset;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_zero(int64_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_zero_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_not_zero(int64_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_not_zero_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_negative(int64_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_negative_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_negative_or_zero(int64_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_negative_or_zero_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_positive(int64_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_positive_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump_if_i32_positive_or_zero(int64_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_if_i32_positive_or_zero_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

struct u7_vm0_instruction u7_vm0_jump(int64_t offset) {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = &u7_vm0_jump_execute},
      .arg1 = {.i64 = offset},
  };
  return result;
}

// u7_vm0_operation

// duplicate

static void u7_vm0_duplicate_i32_execute(struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_duplicate_i32(&state->stack);
}

static void u7_vm0_duplicate_i64_execute(struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_duplicate_i64(&state->stack);
}

static void u7_vm0_duplicate_f32_execute(struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_duplicate_f32(&state->stack);
}

static void u7_vm0_duplicate_f64_execute(struct u7_vm_instruction const* self,
                                         struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_duplicate_f64(&state->stack);
}

U7_VM0_INSTRUCTION_0(duplicate_i32)
U7_VM0_INSTRUCTION_0(duplicate_i64)
U7_VM0_INSTRUCTION_0(duplicate_f32)
U7_VM0_INSTRUCTION_0(duplicate_f64)

// abs

static void u7_vm0_abs_i32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t x = *p;
  if (x == INT32_MIN) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_abs_i32 x=%" PRId32, x);
    state->run = false;
  } else {
    *p = (x >= 0 ? x : -x);
  }
}

static void u7_vm0_abs_i64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t x = *p;
  if (x == INT64_MIN) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_abs_i64 x=%" PRId64, x);
    state->run = false;
  } else {
    *p = (x >= 0 ? x : -x);
  }
}

static void u7_vm0_abs_f32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  float* p = u7_vm_stack_peek_f32(&state->stack);
  float x = *p;
  *p = (x >= 0 ? x : -x);
}

static void u7_vm0_abs_f64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  double* p = u7_vm_stack_peek_f64(&state->stack);
  double x = *p;
  *p = (x >= 0 ? x : -x);
}

U7_VM0_INSTRUCTION_0(abs_i32)
U7_VM0_INSTRUCTION_0(abs_i64)
U7_VM0_INSTRUCTION_0(abs_f32)
U7_VM0_INSTRUCTION_0(abs_f64)

// neg

static void u7_vm0_neg_i32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t x = *p;
  if (x == INT32_MIN) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_neg_i32 x=%" PRId32, x);
    state->run = false;
  } else {
    *p = -x;
  }
}

static void u7_vm0_neg_i64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t x = *p;
  if (x == INT64_MIN) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_neg_i64 x=%" PRId64, x);
    state->run = false;
  } else {
    *p = -x;
  }
}

static void u7_vm0_neg_f32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  float* p = u7_vm_stack_peek_f32(&state->stack);
  float x = *p;
  *p = -x;
}

static void u7_vm0_neg_f64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  double* p = u7_vm_stack_peek_f64(&state->stack);
  double x = *p;
  *p = -x;
}

U7_VM0_INSTRUCTION_0(neg_i32)
U7_VM0_INSTRUCTION_0(neg_i64)
U7_VM0_INSTRUCTION_0(neg_f32)
U7_VM0_INSTRUCTION_0(neg_f64)

// inc

static void u7_vm0_inc_i32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t x = *p;
  int32_t delta = ((struct u7_vm0_instruction const*)self)->arg1.i32;
  if (__builtin_add_overflow(x, delta, p)) {
    state->last_error = u7_errorf(u7_errno_category(), ERANGE,
                                  "integer overflow: u7_vm0_inc_i32 x=%" PRId32
                                  ", delta=%" PRId32,
                                  x, delta);
    state->run = false;
  }
}

static void u7_vm0_inc_i64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t x = *p;
  int64_t delta = ((struct u7_vm0_instruction const*)self)->arg1.i64;
  if (__builtin_add_overflow(x, delta, p)) {
    state->last_error = u7_errorf(u7_errno_category(), ERANGE,
                                  "integer overflow: u7_vm0_inc_i64 x=%" PRId64
                                  ", delta=%" PRId64,
                                  x, delta);
    state->run = false;
  }
}

static void u7_vm0_inc_f32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs += ((struct u7_vm0_instruction const*)self)->arg1.f32;
}

static void u7_vm0_inc_f64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs += ((struct u7_vm0_instruction const*)self)->arg1.f64;
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

// add

static void u7_vm0_add_i32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t lhs = *p;
  if (__builtin_add_overflow(lhs, rhs, p)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_add_i32 lhs=%" PRId32 ", rhs=%" PRId32, lhs,
        rhs);
    state->run = false;
  }
}

static void u7_vm0_add_i64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t lhs = *p;
  if (__builtin_add_overflow(lhs, rhs, p)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_add_i64 lhs=%" PRId64 ", rhs=%" PRId64, lhs,
        rhs);
    state->run = false;
  }
}

static void u7_vm0_add_f32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs += rhs;
}

static void u7_vm0_add_f64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs += rhs;
}

U7_VM0_INSTRUCTION_0(add_i32)
U7_VM0_INSTRUCTION_0(add_i64)
U7_VM0_INSTRUCTION_0(add_f32)
U7_VM0_INSTRUCTION_0(add_f64)

// subtract

static void u7_vm0_subtract_i32_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t lhs = *p;
  if (__builtin_sub_overflow(lhs, rhs, p)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_subtract_i32 lhs=%" PRId32 ", rhs=%" PRId32,
        lhs, rhs);
    state->run = false;
  }
}

static void u7_vm0_subtract_i64_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t lhs = *p;
  if (__builtin_sub_overflow(lhs, rhs, p)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_subtract_i64 lhs=%" PRId64 ", rhs=%" PRId64,
        lhs, rhs);
    state->run = false;
  }
}

static void u7_vm0_subtract_f32_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs -= rhs;
}

static void u7_vm0_subtract_f64_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs -= rhs;
}

U7_VM0_INSTRUCTION_0(subtract_i32)
U7_VM0_INSTRUCTION_0(subtract_i64)
U7_VM0_INSTRUCTION_0(subtract_f32)
U7_VM0_INSTRUCTION_0(subtract_f64)

// multiply

static void u7_vm0_multiply_i32_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t* p = u7_vm_stack_peek_i32(&state->stack);
  int32_t lhs = *p;
  if (__builtin_mul_overflow(lhs, rhs, p)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_multiply_i32 lhs=%" PRId32 ", rhs=%" PRId32,
        lhs, rhs);
    state->run = false;
  }
}

static void u7_vm0_multiply_i64_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t* p = u7_vm_stack_peek_i64(&state->stack);
  int64_t lhs = *p;
  if (__builtin_mul_overflow(lhs, rhs, p)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_multiply_i64 lhs=%" PRId64 ", rhs=%" PRId64,
        lhs, rhs);
    state->run = false;
  }
}

static void u7_vm0_multiply_f32_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs *= rhs;
}

static void u7_vm0_multiply_f64_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs *= rhs;
}

U7_VM0_INSTRUCTION_0(multiply_i32)
U7_VM0_INSTRUCTION_0(multiply_i64)
U7_VM0_INSTRUCTION_0(multiply_f32)
U7_VM0_INSTRUCTION_0(multiply_f64)

// divide

static void u7_vm0_divide_f32_execute(struct u7_vm_instruction const* self,
                                      struct u7_vm_state* state) {
  (void)self;
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float* lhs = u7_vm_stack_peek_f32(&state->stack);
  *lhs /= rhs;
}

static void u7_vm0_divide_f64_execute(struct u7_vm_instruction const* self,
                                      struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double* lhs = u7_vm_stack_peek_f64(&state->stack);
  *lhs /= rhs;
}

U7_VM0_INSTRUCTION_0(divide_f32)
U7_VM0_INSTRUCTION_0(divide_f64)

// rounding

static void u7_vm0_floor_f32_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_floor(*p);
}

static void u7_vm0_floor_f64_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_floor(*p);
}

U7_VM0_INSTRUCTION_0(floor_f32)
U7_VM0_INSTRUCTION_0(floor_f64)

static void u7_vm0_ceil_f32_execute(struct u7_vm_instruction const* self,
                                    struct u7_vm_state* state) {
  (void)self;
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_ceil(*p);
}

static void u7_vm0_ceil_f64_execute(struct u7_vm_instruction const* self,
                                    struct u7_vm_state* state) {
  (void)self;
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_ceil(*p);
}

U7_VM0_INSTRUCTION_0(ceil_f32)
U7_VM0_INSTRUCTION_0(ceil_f64)

static void u7_vm0_round_f32_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_round(*p);
}

static void u7_vm0_round_f64_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_round(*p);
}

U7_VM0_INSTRUCTION_0(round_f32)
U7_VM0_INSTRUCTION_0(round_f64)

static void u7_vm0_trunc_f32_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  float* p = u7_vm_stack_peek_f32(&state->stack);
  *p = __builtin_trunc(*p);
}

static void u7_vm0_trunc_f64_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  double* p = u7_vm_stack_peek_f64(&state->stack);
  *p = __builtin_trunc(*p);
}

U7_VM0_INSTRUCTION_0(trunc_f32)
U7_VM0_INSTRUCTION_0(trunc_f64)

// cast

static void u7_vm0_cast_i32_to_i64_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_i64(&state->stack, u7_vm_stack_pop_i32(&state->stack));
}

static void u7_vm0_cast_i32_to_f32_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f32(&state->stack, u7_vm_stack_pop_i32(&state->stack));
}

static void u7_vm0_cast_i32_to_f64_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f64(&state->stack, u7_vm_stack_pop_i32(&state->stack));
}

static void u7_vm0_cast_i64_to_i32_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)self;
  int64_t x = u7_vm_stack_pop_i64(&state->stack);
  if (x < INT32_MIN || x > INT32_MAX) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_cast_i64_to_i32 x=%" PRId64, x);
    state->run = false;
  } else {
    u7_vm_stack_push_i32(&state->stack, x);
  }
}

static void u7_vm0_cast_i64_to_f32_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f32(&state->stack, u7_vm_stack_pop_i64(&state->stack));
}

static void u7_vm0_cast_i64_to_f64_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f64(&state->stack, u7_vm_stack_pop_i64(&state->stack));
}

static void u7_vm0_cast_f32_to_f64_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f64(&state->stack, u7_vm_stack_pop_f32(&state->stack));
}

static void u7_vm0_cast_f64_to_f32_execute(struct u7_vm_instruction const* self,
                                           struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f32(&state->stack, u7_vm_stack_pop_f64(&state->stack));
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

static void u7_vm0_trunc_f32_to_i32_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  (void)self;
  float x = u7_vm_stack_pop_f32(&state->stack);
  // FIXME(apronchenkov): Make an check exact.
  if (x < (float)INT32_MIN || x > (float)INT32_MAX) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_trunc_f32_to_i32 x=%f", x);
    state->run = false;
  } else {
    u7_vm_stack_push_i32(&state->stack, x);
  }
}

static void u7_vm0_trunc_f32_to_i64_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  (void)self;
  float x = u7_vm_stack_pop_f32(&state->stack);
  // FIXME(apronchenkov): Make an check exact.
  if (x < (float)INT64_MIN || x > (float)INT64_MAX) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_trunc_f32_to_i64 x=%f", x);
    state->run = false;
  } else {
    u7_vm_stack_push_i64(&state->stack, x);
  }
}

static void u7_vm0_trunc_f64_to_i32_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  (void)self;
  double x = u7_vm_stack_pop_f64(&state->stack);
  // FIXME(apronchenkov): Make an check exact.
  if (x < (double)INT32_MIN || x > (double)INT32_MAX) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_trunc_f64_to_i32 x=%f", x);
    state->run = false;
  } else {
    u7_vm_stack_push_i32(&state->stack, x);
  }
}

static void u7_vm0_trunc_f64_to_i64_execute(
    struct u7_vm_instruction const* self, struct u7_vm_state* state) {
  (void)self;
  double x = u7_vm_stack_pop_f64(&state->stack);
  // FIXME(apronchenkov): Make an check exact.
  if (x < (double)INT64_MIN || x > (double)INT64_MAX) {
    state->last_error =
        u7_errorf(u7_errno_category(), ERANGE,
                  "integer overflow: u7_vm0_trunc_f64_to_i64 x=%f", x);
    state->run = false;
  } else {
    u7_vm_stack_push_i64(&state->stack, x);
  }
}

U7_VM0_INSTRUCTION_0(trunc_f32_to_i32)
U7_VM0_INSTRUCTION_0(trunc_f32_to_i64)
U7_VM0_INSTRUCTION_0(trunc_f64_to_i32)
U7_VM0_INSTRUCTION_0(trunc_f64_to_i64)

// print

static void u7_vm0_print_i32_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  printf("%" PRId32, u7_vm_stack_pop_i32(&state->stack));
}

static void u7_vm0_print_i64_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  printf("%" PRId64, u7_vm_stack_pop_i64(&state->stack));
}

static void u7_vm0_print_f32_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  printf("%f", u7_vm_stack_pop_f32(&state->stack));
}

static void u7_vm0_print_f64_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  printf("%f", u7_vm_stack_pop_f64(&state->stack));
}

static void u7_vm0_println_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  (void)state;
  printf("\n");
}

U7_VM0_INSTRUCTION_0(print_i32)
U7_VM0_INSTRUCTION_0(print_i64)
U7_VM0_INSTRUCTION_0(print_f32)
U7_VM0_INSTRUCTION_0(print_f64)
U7_VM0_INSTRUCTION_0(println)

// yield

static void u7_vm0_yield_execute(struct u7_vm_instruction const* self,
                                 struct u7_vm_state* state) {
  (void)self;
  state->run = false;
}

U7_VM0_INSTRUCTION_0(yield)

// dump_state

static void u7_vm0_dump_state_execute(struct u7_vm_instruction const* self,
                                      struct u7_vm_state* state) {
  (void)self;
  printf("ip=%zd  bp=%zd  sp=%zd\n", state->ip, state->stack.base_offset,
         state->stack.top_offset);
}

U7_VM0_INSTRUCTION_0(dump_state)
