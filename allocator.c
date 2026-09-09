#include "@/public/allocator.h"

#include <assert.h>
#include <stdlib.h>

static void* u7_vm_default_allocate(void* data, size_t size) {
  (void)data;
  return malloc(size);
}

static void* u7_vm_default_reallocate(void* data, void* memory, size_t old_size,
                                      size_t new_size) {
  (void)data;
  (void)old_size;
  return realloc(memory, new_size);
}

static void u7_vm_default_deallocate(void* data, void* memory, size_t size) {
  (void)data;
  (void)size;
  free(memory);
}

struct u7_vm_allocator const u7_vm_default_allocator = {
    .allocate_fn = u7_vm_default_allocate,
    .reallocate_fn = u7_vm_default_reallocate,
    .deallocate_fn = u7_vm_default_deallocate,
};

static void* u7_vm_limited_allocate(void* data, size_t size) {
  struct u7_vm_limited_allocator* self = data;
  assert(self->used <= self->limit);
  if (size > self->limit - self->used) {
    return NULL;
  }
  void* memory = self->upstream.allocate_fn(self->upstream.data, size);
  if (memory != NULL) {
    self->used += size;
  }
  return memory;
}

static void* u7_vm_limited_reallocate(void* data, void* memory, size_t old_size,
                                      size_t new_size) {
  struct u7_vm_limited_allocator* self = data;
  assert(old_size <= self->used);
  assert(self->used <= self->limit);
  if (new_size > old_size && new_size - old_size > self->limit - self->used) {
    return NULL;
  }
  void* result = self->upstream.reallocate_fn(self->upstream.data, memory,
                                              old_size, new_size);
  if (result != NULL) {
    self->used += new_size - old_size;
  }
  return result;
}

static void u7_vm_limited_deallocate(void* data, void* memory, size_t size) {
  struct u7_vm_limited_allocator* self = data;
  assert(size <= self->used);
  self->upstream.deallocate_fn(self->upstream.data, memory, size);
  self->used -= size;
}

void u7_vm_limited_allocator_init(struct u7_vm_limited_allocator* self,
                                  struct u7_vm_allocator upstream,
                                  size_t limit) {
  assert(upstream.allocate_fn != NULL);
  assert(upstream.reallocate_fn != NULL);
  assert(upstream.deallocate_fn != NULL);
  self->upstream = upstream;
  self->limit = limit;
  self->used = 0;
}

struct u7_vm_allocator u7_vm_limited_allocator_make(
    struct u7_vm_limited_allocator* self) {
  return (struct u7_vm_allocator){
      .data = self,
      .allocate_fn = u7_vm_limited_allocate,
      .reallocate_fn = u7_vm_limited_reallocate,
      .deallocate_fn = u7_vm_limited_deallocate,
  };
}
