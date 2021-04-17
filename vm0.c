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

// add

static void u7_vm0_add_i32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int32_t rhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t lhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t result;
  if (__builtin_add_overflow(lhs, rhs, &result)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_add_i32 lhs=%" PRId32 ", rhs=%" PRId32, lhs,
        rhs);
    state->run = false;
  } else {
    u7_vm_stack_push_i32(&state->stack, result);
  }
}

static void u7_vm0_add_i64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t lhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t result;
  if (__builtin_add_overflow(lhs, rhs, &result)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_add_i64 lhs=%" PRId64 ", rhs=%" PRId64, lhs,
        rhs);
    state->run = false;
  } else {
    u7_vm_stack_push_i64(&state->stack, result);
  }
}

static void u7_vm0_add_f32_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float lhs = u7_vm_stack_pop_f32(&state->stack);
  u7_vm_stack_push_f32(&state->stack, lhs + rhs);
}

static void u7_vm0_add_f64_execute(struct u7_vm_instruction const* self,
                                   struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double lhs = u7_vm_stack_pop_f64(&state->stack);
  u7_vm_stack_push_f64(&state->stack, lhs + rhs);
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
  int32_t lhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t result;
  if (__builtin_sub_overflow(lhs, rhs, &result)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_subtract_i32 lhs=%" PRId32 ", rhs=%" PRId32,
        lhs, rhs);
    state->run = false;
  } else {
    u7_vm_stack_push_i32(&state->stack, result);
  }
}

static void u7_vm0_subtract_i64_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t lhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t result;
  if (__builtin_sub_overflow(lhs, rhs, &result)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_subtract_i64 lhs=%" PRId64 ", rhs=%" PRId64,
        lhs, rhs);
    state->run = false;
  } else {
    u7_vm_stack_push_i64(&state->stack, result);
  }
}

static void u7_vm0_subtract_f32_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float lhs = u7_vm_stack_pop_f32(&state->stack);
  u7_vm_stack_push_f32(&state->stack, lhs - rhs);
}

static void u7_vm0_subtract_f64_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double lhs = u7_vm_stack_pop_f64(&state->stack);
  u7_vm_stack_push_f64(&state->stack, lhs - rhs);
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
  int32_t lhs = u7_vm_stack_pop_i32(&state->stack);
  int32_t result;
  if (__builtin_mul_overflow(lhs, rhs, &result)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_multiply_i32 lhs=%" PRId32 ", rhs=%" PRId32,
        lhs, rhs);
    state->run = false;
  } else {
    u7_vm_stack_push_i32(&state->stack, result);
  }
}

static void u7_vm0_multiply_i64_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  int64_t rhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t lhs = u7_vm_stack_pop_i64(&state->stack);
  int64_t result;
  if (__builtin_mul_overflow(lhs, rhs, &result)) {
    state->last_error = u7_errorf(
        u7_errno_category(), ERANGE,
        "integer overflow: u7_vm0_multiply_i64 lhs=%" PRId64 ", rhs=%" PRId64,
        lhs, rhs);
    state->run = false;
  } else {
    u7_vm_stack_push_i64(&state->stack, result);
  }
}

static void u7_vm0_multiply_f32_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  float rhs = u7_vm_stack_pop_f32(&state->stack);
  float lhs = u7_vm_stack_pop_f32(&state->stack);
  u7_vm_stack_push_f32(&state->stack, lhs * rhs);
}

static void u7_vm0_multiply_f64_execute(struct u7_vm_instruction const* self,
                                        struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double lhs = u7_vm_stack_pop_f64(&state->stack);
  u7_vm_stack_push_f64(&state->stack, lhs * rhs);
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
  float lhs = u7_vm_stack_pop_f32(&state->stack);
  u7_vm_stack_push_f32(&state->stack, lhs / rhs);
}

static void u7_vm0_divide_f64_execute(struct u7_vm_instruction const* self,
                                      struct u7_vm_state* state) {
  (void)self;
  double rhs = u7_vm_stack_pop_f64(&state->stack);
  double lhs = u7_vm_stack_pop_f64(&state->stack);
  u7_vm_stack_push_f64(&state->stack, lhs / rhs);
}

U7_VM0_INSTRUCTION_0(divide_f32)
U7_VM0_INSTRUCTION_0(divide_f64)

// rounding

static void u7_vm0_floor_f32_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f32(&state->stack,
                       __builtin_floor(u7_vm_stack_pop_f32(&state->stack)));
}

static void u7_vm0_floor_f64_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f64(&state->stack,
                       __builtin_floorl(u7_vm_stack_pop_f64(&state->stack)));
}

U7_VM0_INSTRUCTION_0(floor_f32)
U7_VM0_INSTRUCTION_0(floor_f64)

static void u7_vm0_ceil_f32_execute(struct u7_vm_instruction const* self,
                                    struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f32(&state->stack,
                       __builtin_ceil(u7_vm_stack_pop_f32(&state->stack)));
}

static void u7_vm0_ceil_f64_execute(struct u7_vm_instruction const* self,
                                    struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f64(&state->stack,
                       __builtin_ceill(u7_vm_stack_pop_f64(&state->stack)));
}

U7_VM0_INSTRUCTION_0(ceil_f32)
U7_VM0_INSTRUCTION_0(ceil_f64)

static void u7_vm0_round_f32_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f32(&state->stack,
                       __builtin_round(u7_vm_stack_pop_f32(&state->stack)));
}

static void u7_vm0_round_f64_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f64(&state->stack,
                       __builtin_roundl(u7_vm_stack_pop_f64(&state->stack)));
}

U7_VM0_INSTRUCTION_0(round_f32)
U7_VM0_INSTRUCTION_0(round_f64)

static void u7_vm0_trunc_f32_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f32(&state->stack,
                       __builtin_trunc(u7_vm_stack_pop_f32(&state->stack)));
}

static void u7_vm0_trunc_f64_execute(struct u7_vm_instruction const* self,
                                     struct u7_vm_state* state) {
  (void)self;
  u7_vm_stack_push_f64(&state->stack,
                       __builtin_truncl(u7_vm_stack_pop_f64(&state->stack)));
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
