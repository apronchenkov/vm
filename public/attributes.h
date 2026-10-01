#ifndef U7_VM_ATTRIBUTES_H_
#define U7_VM_ATTRIBUTES_H_

// Optional compiler attributes. Expand to nothing when unsupported.
#if defined(__has_attribute)

#if __has_attribute(always_inline)
// Requests inlining regardless of the compiler's usual inlining heuristics.
#define U7_VM_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#endif

#if __has_attribute(cold)
// Marks functions as unlikely to be called.
#define U7_VM_ATTRIBUTE_COLD __attribute__((cold))
#endif

#if __has_attribute(unused)
// Suppresses warnings when the annotated entity is unused.
#define U7_VM_ATTRIBUTE_MAYBE_UNUSED __attribute__((unused))
#endif

#if __has_attribute(warn_unused_result)
// Warns when a return value is ignored.
#define U7_VM_ATTRIBUTE_NODISCARD __attribute__((warn_unused_result))
#endif

#if __has_attribute(noinline)
// Prevents the function from being inlined.
#define U7_VM_ATTRIBUTE_NOINLINE __attribute__((noinline))
#endif

#if __has_attribute(preserve_none)
// Uses a calling convention with fewer register-preservation requirements.
#define U7_VM_ATTRIBUTE_PRESERVE_NONE __attribute__((preserve_none))
#endif

#endif  // defined(__has_attribute)

#ifndef U7_VM_ATTRIBUTE_ALWAYS_INLINE
#define U7_VM_ATTRIBUTE_ALWAYS_INLINE
#endif

#ifndef U7_VM_ATTRIBUTE_COLD
#define U7_VM_ATTRIBUTE_COLD
#endif

#ifndef U7_VM_ATTRIBUTE_MAYBE_UNUSED
#define U7_VM_ATTRIBUTE_MAYBE_UNUSED
#endif

#ifndef U7_VM_ATTRIBUTE_NODISCARD
#define U7_VM_ATTRIBUTE_NODISCARD
#endif

#ifndef U7_VM_ATTRIBUTE_NOINLINE
#define U7_VM_ATTRIBUTE_NOINLINE
#endif

#ifndef U7_VM_ATTRIBUTE_PRESERVE_NONE
#define U7_VM_ATTRIBUTE_PRESERVE_NONE
#endif

#endif  // U7_VM_ATTRIBUTES_H_
