#include "@/public/allocator.h"

#include "@/public/memory_utils.h"

#include <errno.h>
#include <stdlib.h>

static u7_error u7_vm_default_allocator_alloc(struct u7_vm_allocator* self,
                                              size_t size, void** result) {
  (void)self;
  if ((*result = malloc(size)) == NULL) {
    return u7_errorf(
        u7_errno_category(), ENOMEM,
        "u7_vm_default_allocator_alloc: malloc(%zu): not enough memory", size);
  }
  return u7_ok();
}

static void u7_vm_default_allocator_free(struct u7_vm_allocator* self,
                                         void* ptr) {
  (void)self;
  free(ptr);
}

struct u7_vm_allocator* u7_vm_default_allocator() {
  static struct u7_vm_allocator result = {
      .alignment = u7_vm_alignof(long double),
      .alloc_fn = &u7_vm_default_allocator_alloc,
      .free_fn = &u7_vm_default_allocator_free,
  };
  return &result;
}
