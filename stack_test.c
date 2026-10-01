#include "@/public/stack.h"

#include <errno.h>
#include <github.com/apronchenkov/u7_init/public/testing.h>
#include <stddef.h>
#include <stdint.h>

struct collect_layouts_arg {
  struct u7_vm_stack_frame_layout const* visited[8];
  size_t visited_size;
};

static bool collect_layouts(void* data,
                            struct u7_vm_stack_frame_layout const* layout,
                            void* frame_ptr) {
  (void)frame_ptr;
  struct collect_layouts_arg* self = data;
  U7_ASSERT(self->visited_size < 8);
  self->visited[self->visited_size] = layout;
  self->visited_size += 1;
  return true;
}

U7_TEST(test_iterate_visits_every_frame_outward) {
  struct u7_vm_stack_frame_layout root_layout = {.description = "root"};
  struct u7_vm_stack_frame_layout frame1_layout = {.description = "frame1"};
  struct u7_vm_stack_frame_layout frame2_layout = {.description = "frame2"};

  struct u7_vm_stack stack;
  U7_ASSERT_OK(u7_vm_stack_init(&stack, 0, u7_vm_default_allocator));
  struct u7_vm_stack_frame_cursor cursor =
      u7_vm_stack_load_frame_cursor(&stack);
  U7_ASSERT_OK(u7_vm_stack_push_frame(&stack, &root_layout, NULL, &cursor));
  U7_ASSERT_OK(u7_vm_stack_push_frame(&stack, &frame1_layout, NULL, &cursor));
  U7_ASSERT_OK(u7_vm_stack_push_frame(&stack, &frame2_layout, NULL, &cursor));
  u7_vm_stack_store_frame_cursor(&stack, cursor);

  struct collect_layouts_arg collected = {0};
  u7_vm_stack_iterate(&stack, &collected, collect_layouts);

  U7_ASSERT_EQ(collected.visited_size, 3);
  U7_ASSERT(collected.visited[0] == &frame2_layout);
  U7_ASSERT(collected.visited[1] == &frame1_layout);
  U7_ASSERT(collected.visited[2] == &root_layout);

  u7_vm_stack_destroy(&stack);
}

static bool stop_after_first(void* data,
                             struct u7_vm_stack_frame_layout const* layout,
                             void* frame_ptr) {
  (void)frame_ptr;
  struct collect_layouts_arg* self = data;
  self->visited[self->visited_size] = layout;
  self->visited_size += 1;
  return false;
}

U7_TEST(test_iterate_stops_when_visitor_returns_false) {
  struct u7_vm_stack_frame_layout root_layout = {.description = "root"};
  struct u7_vm_stack_frame_layout frame1_layout = {.description = "frame1"};

  struct u7_vm_stack stack;
  U7_ASSERT_OK(u7_vm_stack_init(&stack, 0, u7_vm_default_allocator));
  struct u7_vm_stack_frame_cursor cursor =
      u7_vm_stack_load_frame_cursor(&stack);
  U7_ASSERT_OK(u7_vm_stack_push_frame(&stack, &root_layout, NULL, &cursor));
  U7_ASSERT_OK(u7_vm_stack_push_frame(&stack, &frame1_layout, NULL, &cursor));
  u7_vm_stack_store_frame_cursor(&stack, cursor);

  struct collect_layouts_arg collected = {0};
  u7_vm_stack_iterate(&stack, &collected, stop_after_first);

  U7_ASSERT_EQ(collected.visited_size, 1);
  U7_ASSERT(collected.visited[0] == &frame1_layout);

  u7_vm_stack_destroy(&stack);
}

static bool verify_descending_values(
    void* data, struct u7_vm_stack_frame_layout const* layout,
    void* frame_ptr) {
  (void)layout;
  int* next_expected = data;
  if (*(int*)frame_ptr != *next_expected) {
    return false;
  }
  *next_expected -= 1;
  return true;
}

U7_TEST(test_growing_the_stack_preserves_frame_contents) {
  struct u7_vm_stack_frame_layout layout = {
      .locals_size = u7_vm_align_size(sizeof(int), U7_VM_DEFAULT_ALIGNMENT),
      .description = "counter",
  };
  struct u7_vm_stack stack;
  U7_ASSERT_OK(u7_vm_stack_init(&stack, 0, u7_vm_default_allocator));

  enum { kFrameCount = 256 };
  for (int i = 0; i < kFrameCount; ++i) {
    struct u7_vm_stack_frame_cursor cursor =
        u7_vm_stack_load_frame_cursor(&stack);
    U7_ASSERT_OK(u7_vm_stack_push_frame(&stack, &layout, NULL, &cursor));
    *(int*)u7_vm_stack_frame_locals(cursor.base) = i;
    u7_vm_stack_store_frame_cursor(&stack, cursor);
  }

  int next_expected = kFrameCount - 1;
  u7_vm_stack_iterate(&stack, &next_expected, verify_descending_values);
  U7_ASSERT_EQ(next_expected, -1);

  u7_vm_stack_destroy(&stack);
}

U7_TEST(test_reserve_is_a_noop_when_capacity_already_sufficient) {
  struct u7_vm_stack stack;
  U7_ASSERT_OK(u7_vm_stack_init(&stack, 128, u7_vm_default_allocator));
  void* const memory = stack.memory;
  U7_ASSERT_OK(u7_vm_stack_reserve(&stack, 64));
  U7_ASSERT_EQ(stack.capacity, 128);
  U7_ASSERT(stack.memory == memory);
  u7_vm_stack_destroy(&stack);
}

U7_TEST(test_reserve_doubles_capacity_when_growth_is_needed) {
  struct u7_vm_stack stack;
  U7_ASSERT_OK(u7_vm_stack_init(&stack, 64, u7_vm_default_allocator));
  U7_ASSERT_OK(u7_vm_stack_reserve(&stack, 65));
  U7_ASSERT_EQ(stack.capacity, 128);
  u7_vm_stack_destroy(&stack);
}

U7_TEST(test_reserve_grows_to_required_capacity_when_doubling_is_not_enough) {
  struct u7_vm_stack stack;
  U7_ASSERT_OK(u7_vm_stack_init(&stack, 64, u7_vm_default_allocator));
  U7_ASSERT_OK(u7_vm_stack_reserve(&stack, 1000));
  U7_ASSERT_EQ(stack.capacity, 1000);
  u7_vm_stack_destroy(&stack);
}

U7_TEST(test_reserve_reports_allocation_failure) {
  struct u7_vm_limited_allocator limited;
  u7_vm_limited_allocator_init(&limited, u7_vm_default_allocator, 64);
  struct u7_vm_stack stack;
  U7_ASSERT_OK(
      u7_vm_stack_init(&stack, 0, u7_vm_limited_allocator_make(&limited)));
  U7_ASSERT_ERROR_CODE(u7_vm_stack_reserve(&stack, 1000), ENOMEM);
  U7_ASSERT_EQ(stack.capacity, 0);
  u7_vm_stack_destroy(&stack);
}

U7_TEST(test_push_frame_reports_allocation_failure) {
  struct u7_vm_limited_allocator limited;
  u7_vm_limited_allocator_init(&limited, u7_vm_default_allocator,
                               U7_VM_STACK_FRAME_HEADER_SIZE);
  struct u7_vm_stack stack;
  U7_ASSERT_OK(
      u7_vm_stack_init(&stack, 0, u7_vm_limited_allocator_make(&limited)));

  struct u7_vm_stack_frame_layout layout = {.description = "frame"};
  struct u7_vm_stack_frame_cursor cursor =
      u7_vm_stack_load_frame_cursor(&stack);
  U7_ASSERT_OK(u7_vm_stack_push_frame(&stack, &layout, NULL, &cursor));
  U7_ASSERT_ERROR_CODE(u7_vm_stack_push_frame(&stack, &layout, NULL, &cursor),
                       ENOMEM);
  u7_vm_stack_store_frame_cursor(&stack, cursor);
  u7_vm_stack_destroy(&stack);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
