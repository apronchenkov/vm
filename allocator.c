#include "@/public/allocator.h"

#include "@/public/memory_utils.h"

#include <stdlib.h>

static void* u7_vm_default_allocator_alloc(struct u7_vm_allocator* self,
                                           size_t size) {
  (void)self;
  return malloc(size);
}

static void u7_vm_default_allocator_free(struct u7_vm_allocator* self,
                                         void* ptr) {
  (void)self;
  free(ptr);
}

struct u7_vm_allocator* u7_vm_default_allocator() {
  static struct u7_vm_allocator result = {
      .alignment = kU7VmStandardAlignment,
      .alloc_fn = &u7_vm_default_allocator_alloc,
      .free_fn = &u7_vm_default_allocator_free,
  };
  return &result;
}
