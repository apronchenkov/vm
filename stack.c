#include "@/public/stack.h"

#include "@/public/memory_utils.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void u7_vm_stack_init(struct u7_vm_stack* self) {
  self->memory = NULL;
  self->base_offset = 0;
  self->top_offset = 0;
  self->capacity = 0;
}

void u7_vm_stack_destroy(struct u7_vm_stack* self) {
  while (self->base_offset != self->top_offset) {
    u7_vm_stack_pop_frame(self);
  }
  free(self->memory);
}

struct u7_vm_stack_reserve_visitor_arg {
  void* old_memory;
  void* new_memory;
};

static bool u7_vm_stack_reserve_visitor(
    void* arg, struct u7_vm_stack_frame_layout const* frame_layout,
    void* frame_ptr) {
  struct u7_vm_stack_reserve_visitor_arg a =
      *(struct u7_vm_stack_reserve_visitor_arg*)arg;
  if (frame_layout->post_realloc_fn) {
    frame_layout->post_realloc_fn(
        frame_layout, frame_ptr,
        u7_vm_memory_add_offset(
            a.new_memory, u7_vm_memory_byte_distance(a.old_memory, frame_ptr)));
  }
  return true;
}

static u7_error u7_vm_stack_reserve(struct u7_vm_stack* self, size_t capacity) {
  if (self->capacity >= capacity) {
    return u7_ok();
  }
  if (capacity < 3 * self->capacity / 2) {
    capacity = 3 * self->capacity / 2;
  }
  void* memory = malloc(capacity);
  if (memory == NULL) {
    return u7_errnof(ENOMEM,
                     "u7_vm_stack_reserve: malloc(%zu): not enough memory",
                     capacity);
  }
  assert(u7_vm_memory_is_aligned(memory, U7_VM_DEFAULT_ALIGNMENT));
  if (self->memory) {
    memcpy(memory, self->memory, self->top_offset);
    struct u7_vm_stack_reserve_visitor_arg a = {.old_memory = self->memory,
                                                .new_memory = memory};
    u7_vm_stack_iterate(self, &a, u7_vm_stack_reserve_visitor);
    free(self->memory);
  }
  self->memory = memory;
  self->capacity = capacity;
  return u7_ok();
}

u7_error u7_vm_stack_push_frame(
    struct u7_vm_stack* self,
    struct u7_vm_stack_frame_layout const* frame_layout) {
  assert(self->top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(frame_layout->locals_size % U7_VM_DEFAULT_ALIGNMENT == 0);
  U7_RETURN_IF_ERROR(u7_vm_stack_reserve(
      self, self->top_offset + U7_VM_STACK_FRAME_HEADER_SIZE +
                frame_layout->locals_size + frame_layout->extra_capacity));
  struct u7_vm_stack_frame_header* const frame_header =
      u7_vm_memory_add_offset(self->memory, self->top_offset);
  frame_header->old_base_offset = self->base_offset;
  frame_header->frame_layout = frame_layout;
  if (frame_layout->init_fn) {
    frame_layout->init_fn(
        frame_layout,
        u7_vm_memory_add_offset(
            self->memory, self->top_offset + U7_VM_STACK_FRAME_HEADER_SIZE));
  }
  self->base_offset = self->top_offset;
  self->top_offset += U7_VM_STACK_FRAME_HEADER_SIZE + frame_layout->locals_size;
  return u7_ok();
}

void u7_vm_stack_pop_frame(struct u7_vm_stack* self) {
  size_t base_offset = self->base_offset;
  size_t top_offset = self->top_offset;
  (void)top_offset;
  assert(base_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(top_offset >= base_offset + sizeof(struct u7_vm_stack_frame_header));
  struct u7_vm_stack_frame_header const frame_header =
      *(struct u7_vm_stack_frame_header*)u7_vm_memory_add_offset(self->memory,
                                                                 base_offset);
  struct u7_vm_stack_frame_layout const* const frame_layout =
      frame_header.frame_layout;
  assert(top_offset >=
         base_offset + U7_VM_DEFAULT_ALIGNMENT + frame_layout->locals_size);
  if (frame_layout->deinit_fn) {
    frame_layout->deinit_fn(
        frame_layout,
        u7_vm_memory_add_offset(
            self->memory, self->base_offset + U7_VM_STACK_FRAME_HEADER_SIZE));
  }
  self->top_offset = self->base_offset;
  self->base_offset = frame_header.old_base_offset;
}

void u7_vm_stack_iterate(struct u7_vm_stack* self, void* arg,
                         u7_vm_stack_visitor_fn_t visitor) {
  size_t base_offset = self->base_offset;
  size_t top_offset = self->top_offset;
  while (base_offset != top_offset) {
    assert(base_offset < top_offset);
    assert(base_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
    assert(top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
    assert(top_offset - base_offset >= sizeof(struct u7_vm_stack_frame_header));
    struct u7_vm_stack_frame_header const frame_header =
        *(struct u7_vm_stack_frame_header*)u7_vm_memory_add_offset(self->memory,
                                                                   base_offset);
    if (!visitor(
            arg, frame_header.frame_layout,
            u7_vm_memory_add_offset(
                self->memory, base_offset + U7_VM_STACK_FRAME_HEADER_SIZE))) {
      break;
    }
    top_offset = base_offset;
    top_offset = frame_header.old_base_offset;
  }
}
