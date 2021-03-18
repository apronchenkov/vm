#include "@/public/stack.h"

#include "@/public/memory_utils.h"

#include <assert.h>

void u7_vm_stack_init(struct u7_vm_stack* self,
                      struct u7_vm_allocator* allocator) {
  assert(allocator->alignment >= kU7VmDefaultAlignment);
  self->base = NULL;
  self->begin = NULL;
  self->size = 0;
  self->capacity = 0;
  self->allocator = allocator;
}

static bool u7_vm_stack_destroy_visitor(
    void* arg, struct u7_vm_memory_layout const* frame_layout,
    void* frame_ptr) {
  (void)arg;
  u7_vm_memory_layout_uninit_memory(frame_layout, frame_ptr);
  return true;
}

void u7_vm_stack_destroy(struct u7_vm_stack* self) {
  u7_vm_stack_iterate(self, NULL, u7_vm_stack_destroy_visitor);
  u7_vm_allocator_free(self->allocator, self->begin);
}

struct u7_vm_stack_reserve_visitor_arg {
  void* old_begin;
  void* new_begin;
};

static bool u7_vm_stack_reserve_visitor(
    void* arg, struct u7_vm_memory_layout const* frame_layout,
    void* frame_ptr) {
  struct u7_vm_stack_reserve_visitor_arg a =
      *(struct u7_vm_stack_reserve_visitor_arg*)arg;
  if (frame_layout->relocate_memory_fn) {
    frame_layout->relocate_memory_fn(
        frame_layout, frame_ptr,
        u7_vm_memory_add_offset(
            a.new_begin, u7_vm_memory_byte_distance(a.old_begin, frame_ptr)));
  }
  return true;
}

u7_error u7_vm_stack_reserve(struct u7_vm_stack* self, size_t capacity) {
  if (self->capacity >= capacity) {
    return u7_ok();
  }
  if (capacity < 3 * self->capacity / 2) {
    capacity = 3 * self->capacity / 2;
  }
  void* begin;
  U7_RETURN_IF_ERROR(u7_vm_allocator_alloc(self->allocator, capacity, &begin));
  memcpy(begin, self->begin, self->size);
  assert(u7_vm_memory_is_aligned(begin, kU7VmDefaultAlignment));
  struct u7_vm_stack_reserve_visitor_arg a = {.old_begin = self->begin,
                                              .new_begin = begin};
  u7_vm_stack_iterate(self, &a, u7_vm_stack_reserve_visitor);
  if (self->base) {
    self->base = u7_vm_memory_add_offset(
        begin, u7_vm_memory_byte_distance(self->begin, self->base));
  }
  u7_vm_allocator_free(self->allocator, self->begin);
  self->begin = begin;
  self->capacity = capacity;
  return u7_ok();
}

u7_error u7_vm_stack_push(struct u7_vm_stack* self,
                          struct u7_vm_memory_layout const* frame_layout,
                          void** result) {
  assert(frame_layout->alignment <= kU7VmDefaultAlignment);
  assert(frame_layout->size % kU7VmDefaultAlignment == 0);
  const size_t size = self->size + frame_layout->size + sizeof(void*);
  U7_RETURN_IF_ERROR(u7_vm_stack_reserve(self, size));
  *result = u7_vm_memory_add_offset(self->begin, self->size);
  u7_vm_memory_layout_init_memory(frame_layout, *result);
  *(void const**)u7_vm_memory_add_offset(self->begin, size - sizeof(void*)) =
      frame_layout;
  self->size = size;
  return u7_ok();
}

void u7_vm_stack_pop(struct u7_vm_stack* self) {
  assert(self->size >= sizeof(void*));
  struct u7_vm_memory_layout const* const frame_layout =
      *(struct u7_vm_memory_layout const**)u7_vm_memory_add_offset(
          self->begin, self->size - sizeof(void*));
  size_t const size = self->size - frame_layout->size - sizeof(void*);
  void* const end = u7_vm_memory_add_offset(self->begin, size);
  u7_vm_memory_layout_uninit_memory(frame_layout, end);
  self->size = size;
}

void u7_vm_stack_iterate(struct u7_vm_stack* self, void* arg,
                         u7_vm_stack_visitor_fn_t visitor) {
  void* const begin = self->begin;
  size_t size = self->size;
  while (size > 0) {
    struct u7_vm_memory_layout const* const frame_layout =
        *(struct u7_vm_memory_layout const**)u7_vm_memory_add_offset(
            begin, size - sizeof(void*));
    size -= frame_layout->size + sizeof(void*);
    if (!visitor(arg, frame_layout, u7_vm_memory_add_offset(begin, size))) {
      return;
    }
  }
}
