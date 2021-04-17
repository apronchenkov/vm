#include "@/public/vm0.h"

#include "@/public/stack_push_pop.h"
#include "@/public/state.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

// u7_vm0_load_constant

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

struct u7_vm0_instruction u7_vm0_add_i32() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_add_i32_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_add_i64() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_add_i64_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_add_f32() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_add_f32_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_add_f64() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_add_f64_execute}};
  return result;
}

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

struct u7_vm0_instruction u7_vm0_subtract_i32() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_subtract_i32_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_subtract_i64() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_subtract_i64_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_subtract_f32() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_subtract_f32_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_subtract_f64() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_subtract_f64_execute}};
  return result;
}

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

struct u7_vm0_instruction u7_vm0_multiply_i32() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_multiply_i32_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_multiply_i64() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_multiply_i64_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_multiply_f32() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_multiply_f32_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_multiply_f64() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_multiply_f64_execute}};
  return result;
}

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

struct u7_vm0_instruction u7_vm0_print_i32() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_print_i32_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_print_i64() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_print_i64_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_print_f32() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_print_f32_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_print_f64() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_print_f64_execute}};
  return result;
}

struct u7_vm0_instruction u7_vm0_println() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_println_execute}};
  return result;
}

// yield

static void u7_vm0_yield_execute(struct u7_vm_instruction const* self,
                                 struct u7_vm_state* state) {
  (void)self;
  state->run = false;
}

struct u7_vm0_instruction u7_vm0_yield() {
  struct u7_vm0_instruction result = {
      .base = {.execute_fn = u7_vm0_yield_execute}};
  return result;
}
