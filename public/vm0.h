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

struct u7_vm0_instruction {
  struct u7_vm_instruction base;
  union u7_vm0_value arg1;
  union u7_vm0_value arg2;
};

struct u7_vm0_instruction u7_vm0_load_constant_i32(int32_t value);
struct u7_vm0_instruction u7_vm0_load_constant_i64(int64_t value);
struct u7_vm0_instruction u7_vm0_load_constant_f32(float value);
struct u7_vm0_instruction u7_vm0_load_constant_f64(double value);

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

struct u7_vm0_instruction u7_vm0_print_i32();
struct u7_vm0_instruction u7_vm0_print_i64();
struct u7_vm0_instruction u7_vm0_print_f32();
struct u7_vm0_instruction u7_vm0_print_f64();
struct u7_vm0_instruction u7_vm0_println();

struct u7_vm0_instruction u7_vm0_yield();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_PL0_H_
