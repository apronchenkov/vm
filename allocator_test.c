#include "@/public/allocator.h"

#include "@/test_allocator.h"

#include <github.com/apronchenkov/u7_init/public/testing.h>
#include <stdint.h>

U7_TEST(test_default_allocator) {
  struct u7_vm_allocator allocator = u7_vm_default_allocator;
  uint8_t* memory = allocator.allocate_fn(allocator.data, 16);
  U7_ASSERT(memory != NULL);
  memory[0] = UINT8_C(0x42);

  memory = allocator.reallocate_fn(allocator.data, memory, 16, 32);
  U7_ASSERT(memory != NULL);
  U7_ASSERT(memory[0] == UINT8_C(0x42));

  allocator.deallocate_fn(allocator.data, memory, 32);
}

U7_TEST(test_limited_allocator) {
  struct u7_vm_test_allocator upstream_state = {0};
  struct u7_vm_limited_allocator limited;
  u7_vm_limited_allocator_init(&limited,
                               u7_vm_test_allocator_make(&upstream_state), 64);
  struct u7_vm_allocator allocator = u7_vm_limited_allocator_make(&limited);

  void* memory = allocator.allocate_fn(allocator.data, 32);
  U7_ASSERT(memory != NULL);
  U7_ASSERT(limited.used == 32);
  U7_ASSERT(upstream_state.used == 32);

  void* grown = allocator.reallocate_fn(allocator.data, memory, 32, 64);
  U7_ASSERT(grown != NULL);
  U7_ASSERT(limited.used == 64);
  U7_ASSERT(upstream_state.used == 64);

  void* failed = allocator.reallocate_fn(allocator.data, grown, 64, 128);
  U7_ASSERT(failed == NULL);
  U7_ASSERT(limited.used == 64);
  U7_ASSERT(upstream_state.used == 64);

  failed = allocator.allocate_fn(allocator.data, 1);
  U7_ASSERT(failed == NULL);
  U7_ASSERT(limited.used == 64);
  U7_ASSERT(upstream_state.used == 64);

  allocator.deallocate_fn(allocator.data, grown, 64);
  U7_ASSERT(limited.used == 0);
  U7_ASSERT(upstream_state.used == 0);
  U7_ASSERT(upstream_state.deallocation_count == 1);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
