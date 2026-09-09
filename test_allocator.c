#include "@/test_allocator.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static bool u7_vm_test_allocator_should_fail(struct u7_vm_test_allocator* self) {
  self->call_count += 1;
  return self->fail_on_call != 0 && self->call_count == self->fail_on_call;
}

static void* u7_vm_test_allocate(void* data, size_t size) {
  struct u7_vm_test_allocator* self = data;
  if (u7_vm_test_allocator_should_fail(self)) {
    return NULL;
  }
  void* const memory = malloc(size);
  if (memory == NULL) {
    abort();  // real OOM, not the simulated failure above
  }
  self->used += size;
  return memory;
}

static void* u7_vm_test_reallocate(void* data, void* memory, size_t old_size,
                                   size_t new_size) {
  struct u7_vm_test_allocator* self = data;
  assert(old_size <= self->used);
  if (u7_vm_test_allocator_should_fail(self)) {
    return NULL;
  }
  if (self->always_move_reallocation) {
    void* const result = malloc(new_size);
    if (result == NULL) {
      abort();  // real OOM, not the simulated failure above
    }
    const size_t copy_size = old_size < new_size ? old_size : new_size;
    if (copy_size != 0) {
      memcpy(result, memory, copy_size);
    }
    free(memory);
    memory = result;
  } else {
    memory = realloc(memory, new_size);
    if (memory == NULL) {
      abort();  // real OOM, not the simulated failure above
    }
  }
  self->used = self->used - old_size + new_size;
  return memory;
}

static void u7_vm_test_deallocate(void* data, void* memory, size_t size) {
  struct u7_vm_test_allocator* self = data;
  assert(size <= self->used);
  free(memory);
  self->used -= size;
  self->deallocation_count += 1;
}

struct u7_vm_allocator u7_vm_test_allocator_make(
    struct u7_vm_test_allocator* self) {
  return (struct u7_vm_allocator){
      .data = self,
      .allocate_fn = u7_vm_test_allocate,
      .reallocate_fn = u7_vm_test_reallocate,
      .deallocate_fn = u7_vm_test_deallocate,
  };
}
