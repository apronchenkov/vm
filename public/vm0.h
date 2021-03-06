#ifndef U7_VM_PL0_H_
#define U7_VM_PL0_H_

#include "@/public/instruction.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

union u7_vm0_value {
  int32_t i32;
  int64_t i64;
  float f32;
  double f64;
};

struct u7_vm0_local_variable {
  int64_t offset;  // non-negative
};

struct u7_vm0_local_label {
  int64_t offset;
};

struct u7_vm0_instruction {
  struct u7_vm_instruction base;
  union u7_vm0_value arg1;
  union u7_vm0_value arg2;
};

struct u7_vm0_instruction u7_vm0_load_constant_i32(int32_t value);
struct u7_vm0_instruction u7_vm0_load_constant_i64(int64_t value);
struct u7_vm0_instruction u7_vm0_load_constant_f32(float value);
struct u7_vm0_instruction u7_vm0_load_constant_f64(double value);

struct u7_vm0_instruction u7_vm0_load_local_i32(
    struct u7_vm0_local_variable var);
struct u7_vm0_instruction u7_vm0_load_local_i64(
    struct u7_vm0_local_variable var);
struct u7_vm0_instruction u7_vm0_load_local_f32(
    struct u7_vm0_local_variable var);
struct u7_vm0_instruction u7_vm0_load_local_f64(
    struct u7_vm0_local_variable var);

struct u7_vm0_instruction u7_vm0_store_local_i32(
    struct u7_vm0_local_variable var);
struct u7_vm0_instruction u7_vm0_store_local_i64(
    struct u7_vm0_local_variable var);
struct u7_vm0_instruction u7_vm0_store_local_f32(
    struct u7_vm0_local_variable var);
struct u7_vm0_instruction u7_vm0_store_local_f64(
    struct u7_vm0_local_variable var);

struct u7_vm0_instruction u7_vm0_compare_i32();
struct u7_vm0_instruction u7_vm0_compare_i64();
struct u7_vm0_instruction u7_vm0_compare_f32();
struct u7_vm0_instruction u7_vm0_compare_f64();

struct u7_vm0_instruction u7_vm0_jump_if_i32_zero(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i32_not_zero(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i32_negative(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i32_positive(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i32_negative_or_zero(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i32_positive_or_zero(
    struct u7_vm0_local_label label);

struct u7_vm0_instruction u7_vm0_jump_if_i64_zero(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i64_not_zero(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i64_negative(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i64_positive(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i64_negative_or_zero(
    struct u7_vm0_local_label label);
struct u7_vm0_instruction u7_vm0_jump_if_i64_positive_or_zero(
    struct u7_vm0_local_label label);

struct u7_vm0_instruction u7_vm0_jump(struct u7_vm0_local_label label);

struct u7_vm0_instruction u7_vm0_duplicate_i32();
struct u7_vm0_instruction u7_vm0_duplicate_i64();
struct u7_vm0_instruction u7_vm0_duplicate_f32();
struct u7_vm0_instruction u7_vm0_duplicate_f64();

struct u7_vm0_instruction u7_vm0_bitwise_not_i32();
struct u7_vm0_instruction u7_vm0_bitwise_not_i64();
struct u7_vm0_instruction u7_vm0_bitwise_and_i32();
struct u7_vm0_instruction u7_vm0_bitwise_and_i64();
struct u7_vm0_instruction u7_vm0_bitwise_or_i32();
struct u7_vm0_instruction u7_vm0_bitwise_or_i64();
struct u7_vm0_instruction u7_vm0_bitwise_xor_i32();
struct u7_vm0_instruction u7_vm0_bitwise_xor_i64();

struct u7_vm0_instruction u7_vm0_abs_i32();
struct u7_vm0_instruction u7_vm0_abs_i64();
struct u7_vm0_instruction u7_vm0_abs_f32();
struct u7_vm0_instruction u7_vm0_abs_f64();

struct u7_vm0_instruction u7_vm0_neg_i32();
struct u7_vm0_instruction u7_vm0_neg_i64();
struct u7_vm0_instruction u7_vm0_neg_f32();
struct u7_vm0_instruction u7_vm0_neg_f64();

struct u7_vm0_instruction u7_vm0_inc_i32(int32_t delta);
struct u7_vm0_instruction u7_vm0_inc_i64(int64_t delta);
struct u7_vm0_instruction u7_vm0_inc_f32(float delta);
struct u7_vm0_instruction u7_vm0_inc_f64(double delta);

struct u7_vm0_instruction u7_vm0_inc_local_i64(struct u7_vm0_local_variable var,
                                               int64_t delta);

struct u7_vm0_instruction u7_vm0_add_i32();
struct u7_vm0_instruction u7_vm0_add_i64();
struct u7_vm0_instruction u7_vm0_add_f32();
struct u7_vm0_instruction u7_vm0_add_f64();

struct u7_vm0_instruction u7_vm0_subtract_i32();
struct u7_vm0_instruction u7_vm0_subtract_i64();
struct u7_vm0_instruction u7_vm0_subtract_f32();
struct u7_vm0_instruction u7_vm0_subtract_f64();

struct u7_vm0_instruction u7_vm0_multiply_i32();
struct u7_vm0_instruction u7_vm0_multiply_i64();
struct u7_vm0_instruction u7_vm0_multiply_f32();
struct u7_vm0_instruction u7_vm0_multiply_f64();

struct u7_vm0_instruction u7_vm0_divide_f32();
struct u7_vm0_instruction u7_vm0_divide_f64();

struct u7_vm0_instruction u7_vm0_floordiv_u32();
struct u7_vm0_instruction u7_vm0_floordiv_u64();

struct u7_vm0_instruction u7_vm0_floormod_u32();
struct u7_vm0_instruction u7_vm0_floormod_u64();

struct u7_vm0_instruction u7_vm0_floormod_local_u64(
    struct u7_vm0_local_variable lhs, struct u7_vm0_local_variable rhs);

struct u7_vm0_instruction u7_vm0_floor_f32();
struct u7_vm0_instruction u7_vm0_floor_f64();
struct u7_vm0_instruction u7_vm0_ceil_f32();
struct u7_vm0_instruction u7_vm0_ceil_f64();
struct u7_vm0_instruction u7_vm0_round_f32();
struct u7_vm0_instruction u7_vm0_round_f64();
struct u7_vm0_instruction u7_vm0_trunc_f32();
struct u7_vm0_instruction u7_vm0_trunc_f64();

struct u7_vm0_instruction u7_vm0_sqrt_f32();
struct u7_vm0_instruction u7_vm0_sqrt_f64();

struct u7_vm0_instruction u7_vm0_cast_i32_to_i64();
struct u7_vm0_instruction u7_vm0_cast_i32_to_f32();
struct u7_vm0_instruction u7_vm0_cast_i32_to_f64();
struct u7_vm0_instruction u7_vm0_cast_i64_to_i32();
struct u7_vm0_instruction u7_vm0_cast_i64_to_f32();
struct u7_vm0_instruction u7_vm0_cast_i64_to_f64();
struct u7_vm0_instruction u7_vm0_cast_f32_to_f64();
struct u7_vm0_instruction u7_vm0_cast_f64_to_f32();

struct u7_vm0_instruction u7_vm0_trunc_f32_to_i32();
struct u7_vm0_instruction u7_vm0_trunc_f32_to_i64();
struct u7_vm0_instruction u7_vm0_trunc_f64_to_i32();
struct u7_vm0_instruction u7_vm0_trunc_f64_to_i64();

struct u7_vm0_instruction u7_vm0_print_i32();
struct u7_vm0_instruction u7_vm0_print_i64();
struct u7_vm0_instruction u7_vm0_print_f32();
struct u7_vm0_instruction u7_vm0_print_f64();
struct u7_vm0_instruction u7_vm0_println();

struct u7_vm0_instruction u7_vm0_yield();

struct u7_vm0_instruction u7_vm0_dump_state();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_PL0_H_
