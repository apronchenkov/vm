# RFC-10: Purpose and Scope

Status: draft

## Motivation

Writing a bytecode VM requires solving several recurring problems: keeping
instruction dispatch fast, call conventions, error handling, and stack
unwinding.

The purpose of `u7_vm` is to provide the common mechanics for stack-based
bytecode VMs built around explicit call frames and structural exception
unwinding. Where we believe one design is clearly preferable, `u7_vm`
implements it. Where the trade-offs are less clear, we prefer to keep the
design customizable.

For example, `u7_vm` is intentionally agnostic about value semantics. It does
not know what an `i32` or `list` is, perform garbage collection, or parse or
assemble bytecode. Those concerns remain specific to each VM.

A concrete VM, such as VM0, uses `u7_vm` and provides:

* the instruction table;
* the value types and their ownership rules;
* a loader or assembler that produces valid instruction streams and layout
  metadata.

## Goals

1. **Minimal performance overhead.** Frame push/pop, call/return, and
   exception dispatch are on the hot path and should compile close to what a
   hand-written interpreter would do.
2. **Extensible without changing the core.** Adding an instruction or value
   type must not require changing the core VM. The core knows about frames,
   offsets, and control transfer, but not instruction or value semantics.
3. **No imposed value representation.** Scalars, tagged unions,
   reference-counted handles, and garbage-collected handles are all choices
   made by the concrete VM. RFC-12 leaves garbage collection explicitly open
   rather than deciding it here.

## Non-goals

* A concrete instruction set -- that is the responsibility of a VM built on
  top of this framework.
* A compiler or assembler front end -- these are specific to each instruction
  set.
* Garbage collection as a built-in mechanism. If needed, it can be layered on
  top as part of the value representation; see RFC-12.
* Concurrent execution of a single `u7_vm_state`.

## Relationship to other RFCs

* RFC-1 (terminology) applies unchanged.
* RFC-2 and RFC-3 are superseded and retained for historical reference,
  although much of their content remains valid.
