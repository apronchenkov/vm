#ifndef U7_VM_PL0_H_
#define U7_VM_PL0_H_

#include "@/public/instruction.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm0_load_constant {
  struct u7_vm_instruction base;
  union {
    int32_t i32;
    int64_t i64;
    float f32;
    double f64;
  } value;
};

struct u7_vm0_load_constant u7_vm0_load_constant_i32(int32_t value);
struct u7_vm0_load_constant u7_vm0_load_constant_i64(int64_t value);
struct u7_vm0_load_constant u7_vm0_load_constant_f32(float value);
struct u7_vm0_load_constant u7_vm0_load_constant_f64(double value);

struct u7_vm0_operation {
  struct u7_vm_instruction base;
};

struct u7_vm0_operation u7_vm0_operation_add_i32();
struct u7_vm0_operation u7_vm0_operation_add_i64();
struct u7_vm0_operation u7_vm0_operation_add_f32();
struct u7_vm0_operation u7_vm0_operation_add_f64();

struct u7_vm0_operation u7_vm0_operation_print_i32();
struct u7_vm0_operation u7_vm0_operation_print_i64();
struct u7_vm0_operation u7_vm0_operation_print_f32();
struct u7_vm0_operation u7_vm0_operation_print_f64();
struct u7_vm0_operation u7_vm0_operation_println();

struct u7_vm0_operation u7_vm0_operation_yield();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_PL0_H_
