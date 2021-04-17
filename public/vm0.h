#ifndef U7_VM_PL0_H_
#define U7_VM_PL0_H_

#include "@/public/instruction.h"

#include <stddef.h>
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

struct u7_vm0_instruction {
  struct u7_vm_instruction base;
  union u7_vm0_value arg1;
  union u7_vm0_value arg2;
};

struct u7_vm0_instruction u7_vm0_load_constant_i32(int32_t value);
struct u7_vm0_instruction u7_vm0_load_constant_i64(int64_t value);
struct u7_vm0_instruction u7_vm0_load_constant_f32(float value);
struct u7_vm0_instruction u7_vm0_load_constant_f64(double value);

struct u7_vm0_instruction u7_vm0_load_local_i32(size_t offset);
struct u7_vm0_instruction u7_vm0_load_local_i64(size_t offset);
struct u7_vm0_instruction u7_vm0_load_local_f32(size_t offset);
struct u7_vm0_instruction u7_vm0_load_local_f64(size_t offset);

struct u7_vm0_instruction u7_vm0_store_local_i32(size_t offset);
struct u7_vm0_instruction u7_vm0_store_local_i64(size_t offset);
struct u7_vm0_instruction u7_vm0_store_local_f32(size_t offset);
struct u7_vm0_instruction u7_vm0_store_local_f64(size_t offset);

struct u7_vm0_instruction u7_vm0_jump(int64_t offset);

struct u7_vm0_instruction u7_vm0_duplicate_i32();
struct u7_vm0_instruction u7_vm0_duplicate_i64();
struct u7_vm0_instruction u7_vm0_duplicate_f32();
struct u7_vm0_instruction u7_vm0_duplicate_f64();

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

struct u7_vm0_instruction u7_vm0_floor_f32();
struct u7_vm0_instruction u7_vm0_floor_f64();
struct u7_vm0_instruction u7_vm0_ceil_f32();
struct u7_vm0_instruction u7_vm0_ceil_f64();
struct u7_vm0_instruction u7_vm0_round_f32();
struct u7_vm0_instruction u7_vm0_round_f64();
struct u7_vm0_instruction u7_vm0_trunc_f32();
struct u7_vm0_instruction u7_vm0_trunc_f64();

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
