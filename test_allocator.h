#ifndef U7_VM_TEST_ALLOCATOR_H_
#define U7_VM_TEST_ALLOCATOR_H_

#include "@/public/allocator.h"

#include <stdbool.h>
#include <stddef.h>

// A deterministic test allocator backed by malloc/realloc/free. Set
// fail_on_call to make the call_count'th allocate/reallocate call return
// NULL, to exercise allocation-failure paths; set always_move_reallocation
// to force every reallocation through malloc+copy+free instead of a
// possible in-place growth, to exercise that path specifically. Tracks live
// byte usage (used) and deallocation_count for assertions.
struct u7_vm_test_allocator {
  size_t call_count;
  size_t fail_on_call;
  size_t used;
  size_t deallocation_count;
  bool always_move_reallocation;
};

// Returns an allocator interface backed by self.
struct u7_vm_allocator u7_vm_test_allocator_make(
    struct u7_vm_test_allocator* self);

#endif  // U7_VM_TEST_ALLOCATOR_H_
