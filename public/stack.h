#ifndef U7_VM_STACK_H_
#define U7_VM_STACK_H_

#include "@/public/allocator.h"
#include "@/public/attributes.h"
#include "@/public/memory_utils.h"

#include <assert.h>
#include <errno.h>
#include <github.com/apronchenkov/u7_init/public/init.h>
#include <github.com/apronchenkov/u7_init/public/math.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_vm_state;
struct u7_vm_instruction;
struct u7_vm_stack_frame_layout;

// Initializes the frame's locals.
typedef void (*u7_vm_stack_frame_layout_init_fn_t)(
    struct u7_vm_stack_frame_layout const* self, void* locals);

// Deinitializes the frame's locals.
typedef void (*u7_vm_stack_frame_layout_deinit_fn_t)(
    struct u7_vm_stack_frame_layout const* self, void* locals);

// Result of consulting a frame's exception handler.
enum u7_vm_exception_handler_action {
  // Unwind the frame that is current after the handler returns.
  U7_VM_EXCEPTION_HANDLER_ACTION_UNWIND,
  // Resume execution at `state->ip` using the state left by the handler.
  U7_VM_EXCEPTION_HANDLER_ACTION_RECOVER,
};

// Handles an exception using the given execution state.
//
// Called while `state->status` is U7_VM_STATE_STATUS_EXCEPTION.
// `state->ip` identifies the instruction associated with the exception in the
// current frame: the instruction that raised it in the originating frame, or
// the instruction at which the frame was suspended after unwinding a callee.
typedef enum u7_vm_exception_handler_action (*u7_vm_exception_handler_fn_t)(
    struct u7_vm_state* state, void* data);

// A stack frame layout.
//
// NOTE: All values stored on the stack, including this frame's locals, must be
// trivially relocatable as defined by RFC-2 GH-1. Growing the stack may move
// them with a plain byte copy and performs no per-frame fixup.
struct u7_vm_stack_frame_layout {
  size_t locals_size;
  size_t extra_capacity;
  u7_vm_stack_frame_layout_init_fn_t /*nullable*/ init_fn;
  u7_vm_stack_frame_layout_deinit_fn_t /*nullable*/ deinit_fn;
  u7_vm_exception_handler_fn_t /*nullable*/ exception_handler_fn;
  void* exception_handler_data;
  const char* description;
};

// A stack frame's header.
struct u7_vm_stack_frame_header {
  size_t previous_base_delta;
  struct u7_vm_stack_frame_layout const* /*nonnull*/ frame_layout;
  struct u7_vm_instruction const* /*nullable*/ return_ip;
};

enum {
  U7_VM_STACK_FRAME_HEADER_SIZE =
      (sizeof(struct u7_vm_stack_frame_header) + U7_VM_DEFAULT_ALIGNMENT - 1) &
  -(size_t)U7_VM_DEFAULT_ALIGNMENT
};

//   ...
// <frame start>
//   return_instruction_address
//   return_value_address  -- when present
// base:
//   stack_frame_header
//   locals                -- locals_size bytes
// locals_end:             == base + header size + locals_size
//   outgoing call area    -- extra_capacity bytes
// extra_end:              == locals_end + extra_capacity
//
// While this frame is active, the live cursor's top stays within
// [locals_end, extra_end], inclusive.

struct u7_vm_stack {
  void* memory;

  // Byte offsets from memory. May be stale while a cursor is live.
  size_t base_offset;  // Current frame's base.
  size_t top_offset;   // Stack top.

  size_t capacity;  // Allocated storage size in bytes.
  struct u7_vm_allocator allocator;
};

// Current frame base and live stack top, threaded through dispatch.
//
// The live cursor is authoritative. Use u7_vm_stack_store_frame_cursor
// to update the stack's stored offsets before calling operations that
// require them to be current.
struct u7_vm_stack_frame_cursor {
  void* /*nonnull*/ base;  // Current frame's header.
  void* /*nonnull*/ top;   // End of the live stack contents.
};

// Initializes the stack structure.
u7_error u7_vm_stack_init(struct u7_vm_stack* self, size_t capacity,
                          struct u7_vm_allocator allocator);

// Deinitializes the remaining frames and releases the stack's storage.
// Requires the stored position to be current.
void u7_vm_stack_destroy(struct u7_vm_stack* self);

// Ensures capacity for at least `required_capacity` bytes on success.
// Does nothing if the current capacity is sufficient.
//
// Store the live cursor before calling and reload it afterward.
// Growth may invalidate other pointers into the stack.
//
// For better performance, callers are strongly advised to check capacity
// first and call this function only when growth is needed.
__attribute__((cold)) u7_error u7_vm_stack_reserve(struct u7_vm_stack* self,
                                                   size_t required_capacity);

// Frame accessors do not read the stack's stored offsets.
// `base` must point to an existing frame in the current allocation.

// Returns the frame's layout.
static inline struct u7_vm_stack_frame_layout const* u7_vm_stack_frame_layout(
    void const* base) {
  assert(u7_vm_memory_is_aligned(base, U7_VM_DEFAULT_ALIGNMENT));
  return ((struct u7_vm_stack_frame_header const*)base)->frame_layout;
}

// Returns a pointer to the frame's locals.
static inline void* u7_vm_stack_frame_locals(void* base) {
  assert(u7_vm_memory_is_aligned(base, U7_VM_DEFAULT_ALIGNMENT));
  return u7_vm_memory_add_offset(base, U7_VM_STACK_FRAME_HEADER_SIZE);
}

// Returns the end of the frame's locals, where its extra capacity begins.
static inline void* u7_vm_stack_frame_locals_end(void* base) {
  return u7_vm_memory_add_offset(u7_vm_stack_frame_locals(base),
                                 u7_vm_stack_frame_layout(base)->locals_size);
}

// Returns the end of the frame's extra capacity.
static inline void* u7_vm_stack_frame_extra_end(void* base) {
  struct u7_vm_stack_frame_layout const* const layout =
      u7_vm_stack_frame_layout(base);
  return u7_vm_memory_add_offset(u7_vm_stack_frame_locals(base),
                                 layout->locals_size + layout->extra_capacity);
}

// Loads a cursor from the stack's stored offsets.
// Requires the stored position to be current.
//
// After stack initialization, use this to obtain the cursor for pushing
// the root frame.
static inline struct u7_vm_stack_frame_cursor u7_vm_stack_load_frame_cursor(
    struct u7_vm_stack const* self) {
  return (struct u7_vm_stack_frame_cursor){
      .base = u7_vm_memory_add_offset(self->memory, self->base_offset),
      .top = u7_vm_memory_add_offset(self->memory, self->top_offset)};
}

// Stores the live cursor's position in `self->base_offset` and
// `self->top_offset`. The cursor must refer to this stack's current
// allocation.
static inline void u7_vm_stack_store_frame_cursor(
    struct u7_vm_stack* self, struct u7_vm_stack_frame_cursor cursor) {
  self->base_offset = (size_t)((char*)cursor.base - (char*)self->memory);
  self->top_offset = (size_t)((char*)cursor.top - (char*)self->memory);
}

// Moves the top by `delta_bytes` without modifying the stored bytes.
// Positive values advance it; negative values retract it.
//
// The top must remain within [locals_end, extra_end], inclusive.
// Returns the updated cursor with its base unchanged.
U7_VM_ATTRIBUTE_NODISCARD static inline struct u7_vm_stack_frame_cursor
u7_vm_stack_frame_cursor_adjust_top(struct u7_vm_stack_frame_cursor cursor,
                                    ptrdiff_t delta_bytes) {
  char* const top = (char*)cursor.top;
  assert(top >= (char*)u7_vm_stack_frame_locals_end(cursor.base));
  assert(top <= (char*)u7_vm_stack_frame_extra_end(cursor.base));
  assert(delta_bytes >= (char*)u7_vm_stack_frame_locals_end(cursor.base) - top);
  assert(delta_bytes <= (char*)u7_vm_stack_frame_extra_end(cursor.base) - top);

  cursor.top = top + delta_bytes;
  return cursor;
}

// Pushes a frame at `cursor->top` and updates `*cursor` to the new frame.
// The first frame pushed onto an empty stack is the root.
//
// May grow the allocation, invalidating pointers into the stack.
// Stores and reloads `*cursor` when growth is needed.
//
// On error, leaves `*cursor` unchanged and valid. The stored offsets may
// have been updated; they need not match the cursor after a successful push.
//
// The stack treats `return_ip` as opaque. u7_vm_stack_pop_frame returns it
// unchanged when the frame is popped.
static inline u7_error u7_vm_stack_push_frame(
    struct u7_vm_stack* self,
    struct u7_vm_stack_frame_layout const* frame_layout,
    struct u7_vm_instruction const* return_ip,
    struct u7_vm_stack_frame_cursor* cursor) {
  void* top = cursor->top;
  size_t const top_offset = (size_t)((char*)top - (char*)self->memory);
  assert(top_offset % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(frame_layout->locals_size % U7_VM_DEFAULT_ALIGNMENT == 0);
  assert(top_offset <= self->capacity);

  bool overflow = false;
  const size_t new_top_offset = U7_ADD_OVERFLOW_U64(
      top_offset,
      U7_ADD_OVERFLOW_U64((size_t)U7_VM_STACK_FRAME_HEADER_SIZE,
                          frame_layout->locals_size, &overflow),
      &overflow);
  const size_t required_capacity = U7_ADD_OVERFLOW_U64(
      new_top_offset, frame_layout->extra_capacity, &overflow);
  if (overflow) {
    return u7_errnof(EOVERFLOW, "u7_vm_stack_push_frame: size overflow");
  }
  if (U7_UNLIKELY(required_capacity > self->capacity)) {
    u7_vm_stack_store_frame_cursor(self, *cursor);
    U7_RETURN_IF_ERROR(u7_vm_stack_reserve(self, required_capacity));
    // Reload both pointers after possible relocation.
    *cursor = u7_vm_stack_load_frame_cursor(self);
    top = cursor->top;
  }
  void* const base = cursor->base;

  struct u7_vm_stack_frame_header* const frame_header =
      (struct u7_vm_stack_frame_header*)top;
  frame_header->previous_base_delta = (size_t)((char*)top - (char*)base);
  frame_header->frame_layout = frame_layout;
  frame_header->return_ip = return_ip;
  if (frame_layout->init_fn) {
    frame_layout->init_fn(
        frame_layout,
        u7_vm_memory_add_offset(top, U7_VM_STACK_FRAME_HEADER_SIZE));
  }
  *cursor = (struct u7_vm_stack_frame_cursor){
      .base = top,
      .top = u7_vm_memory_add_offset(self->memory, new_top_offset)};
  return u7_ok();
}

// Pops the current frame, runs its deinitializer, and returns its `return_ip`.
// The stack must not be empty.
//
// Restores the previous frame's base and uses the popped frame's base
// as its top.
// Popping the root frame leaves an empty cursor at the allocation's start.
// Does not update the stack's stored offsets.
static inline struct u7_vm_instruction const* u7_vm_stack_pop_frame(
    struct u7_vm_stack_frame_cursor* cursor) {
  assert(cursor->top != cursor->base);
  assert(u7_vm_memory_is_aligned(cursor->base, U7_VM_DEFAULT_ALIGNMENT));
  struct u7_vm_stack_frame_header const frame_header =
      *(struct u7_vm_stack_frame_header const*)cursor->base;
  if (frame_header.frame_layout->deinit_fn) {
    frame_header.frame_layout->deinit_fn(
        frame_header.frame_layout, u7_vm_stack_frame_locals(cursor->base));
  }
  void* const new_top = cursor->base;
  void* const new_base = (char*)cursor->base - frame_header.previous_base_delta;
  *cursor = (struct u7_vm_stack_frame_cursor){.base = new_base, .top = new_top};
  return frame_header.return_ip;
}

// Returns a pointer to the globals stored in the root frame.
// Requires the root frame to exist and the stored position to be current.
static inline void* u7_vm_stack_globals(struct u7_vm_stack* self) {
  assert(self->top_offset >= U7_VM_STACK_FRAME_HEADER_SIZE);
  assert(self->top_offset >=
         U7_VM_STACK_FRAME_HEADER_SIZE +
             u7_vm_stack_frame_layout(self->memory)->locals_size);
  return u7_vm_stack_frame_locals(self->memory);
}

// Return false to stop iteration.
typedef bool (*u7_vm_stack_visitor_fn_t)(
    void* data, struct u7_vm_stack_frame_layout const* frame_layout,
    void* base);

// Visits frames from the current frame through the root.
// Requires the stored position to be current.
// Stops after visiting the root or when the visitor returns false.
void u7_vm_stack_iterate(struct u7_vm_stack* self, void* data,
                         u7_vm_stack_visitor_fn_t visitor);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_VM_STACK_H_
