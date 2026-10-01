#include "@/public/stack.h"

#include "@/public/memory_utils.h"

#include <assert.h>
#include <errno.h>
#include <github.com/apronchenkov/u7_init/public/math.h>
#include <stdint.h>

u7_error u7_vm_stack_init(struct u7_vm_stack* self, size_t capacity,
                          struct u7_vm_allocator allocator) {
  void* memory = NULL;
  if (capacity != 0) {
    memory = allocator.allocate_fn(allocator.data, capacity);
    if (memory == NULL) {
      return u7_errnof(ENOMEM,
                       "u7_vm_stack_init: allocate(%zu): not enough memory",
                       capacity);
    }
  }
  self->memory = memory;
  self->base_offset = 0;
  self->top_offset = 0;
  self->capacity = capacity;
  self->allocator = allocator;
  return u7_ok();
}

void u7_vm_stack_destroy(struct u7_vm_stack* self) {
  struct u7_vm_stack_frame_cursor cursor = u7_vm_stack_load_frame_cursor(self);
  while (cursor.base != cursor.top) {
    (void)u7_vm_stack_pop_frame(&cursor);
  }
  if (self->memory != NULL) {
    self->allocator.deallocate_fn(self->allocator.data, self->memory,
                                  self->capacity);
  }
  self->memory = NULL;
  self->capacity = 0;
}

u7_error u7_vm_stack_reserve(struct u7_vm_stack* self,
                             size_t required_capacity) {
  if (required_capacity <= self->capacity) {
    return u7_ok();
  }

  bool overflow = false;
  size_t new_capacity = U7_MUL_OVERFLOW_U64(self->capacity, 2, &overflow);
  if (overflow) {
    new_capacity = SIZE_MAX;
  } else if (new_capacity < required_capacity) {
    new_capacity = required_capacity;
  }

  void* memory = self->allocator.reallocate_fn(
      self->allocator.data, self->memory, self->capacity, new_capacity);
  if (memory == NULL) {
    return u7_errnof(ENOMEM,
                     "u7_vm_stack_reserve: realloc(%zu): not enough memory",
                     new_capacity);
  }
  assert(u7_vm_memory_is_aligned(memory, U7_VM_DEFAULT_ALIGNMENT));
  self->memory = memory;
  self->capacity = new_capacity;
  return u7_ok();
}

void u7_vm_stack_iterate(struct u7_vm_stack* self, void* data,
                         u7_vm_stack_visitor_fn_t visitor) {
  if (self->base_offset == self->top_offset) {
    return;  // nothing has been pushed yet
  }
  size_t base_offset = self->base_offset;
  for (;;) {
    assert(base_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
    struct u7_vm_stack_frame_header const frame_header =
        *(struct u7_vm_stack_frame_header*)u7_vm_memory_add_offset(self->memory,
                                                                   base_offset);
    if (!visitor(
            data, frame_header.frame_layout,
            u7_vm_memory_add_offset(
                self->memory, base_offset + U7_VM_STACK_FRAME_HEADER_SIZE))) {
      break;
    }
    if (frame_header.previous_base_delta == 0) {
      break;  // just visited the root
    }
    base_offset -= frame_header.previous_base_delta;
  }
}
