# RFC-13: Instruction Dispatch and Call Optimisation

Status: draft

## 1. Goal

Describe the techniques used to minimise dispatch and call/return overhead
while allowing instruction execution functions to be compiled
independently.

## 2. Instruction dispatch

### 2.1. Tail-call chain

Each instruction's execution function, `execute_fn`, either tail-calls the
next instruction's `execute_fn` using `musttail` or stops the instruction
chain.

This avoids returning to a central dispatch loop between instructions.
`musttail` prevents the instruction chain from overflowing the native call
stack.

### 2.2. Instruction-pointer contract

Each instruction receives the current instruction pointer, `ip`, as an
argument, while `state->ip` may be stale.

Its `execute_fn` is responsible for updating `state->ip`, but can delegate
the update to another instruction's `execute_fn` by chaining a call to it.
This optimisation avoids updating the field on every dispatch. The
instruction that stops the chain must set `state->ip` to the position
required by its semantics before returning.

### 2.3. Frame-base contract

Each instruction receives the current frame's base pointer, `base`, as an
argument, corresponding to `u7_vm_stack_frame_base(state->stack)`. This
allows instruction bodies to access locals and the frame layout without
going through `state->stack`.

Instructions that push or pop frames must recompute `base` before passing
it to the next instruction in the chain. Other instructions can forward
it unchanged.

In synthetic benchmarks, passing `base` reduced runtime by up to 9%.
The effect on real programs may differ.

### 2.4. Calling convention

Use `__attribute__((preserve_none))` where supported to reduce
register-preservation requirements; otherwise, use the ordinary calling
convention.

In a synthetic benchmark, this reduced runtime by approximately 4-9%. The
benefit may be smaller in real programs.

### 2.5. Compiler requirements

The implementation requires `musttail` support and has been built and
tested with Clang. GCC supports `musttail` starting with version 15, but
this implementation has not been validated with GCC.

Support for the optional `preserve_none` convention depends on the
compiler version and target.

## 3. Instruction structure

### 3.1. Instruction interface

An instruction is represented by a function pointer, `execute_fn`, and a
pointer to its data, `data`. Execution functions share a uniform signature
and calling convention.

### 3.2. Entry point and implementation body

A common implementation separates `execute_fn` into two layers:

- An entry point with the uniform signature and calling convention
  required by the instruction interface. `musttail` requires compatible
  caller and callee signatures and matching calling conventions.
- An implementation body that receives `data` cast to a concrete
  `self_type`. `__attribute__((always_inline))` requests that this body be
  inlined into `execute_fn`.

### 3.3. Cold failure paths

An instruction with substantial failure-handling code can extract that
code into a separate function using the same signature and calling
convention as `execute_fn`. On failure, `execute_fn` tail-calls this
function using `musttail`.

This separation helps keep failure-handling code out of the normal
execution path.

## 4. Contiguous stack with geometric growth

`u7_vm_stack_reserve` grows the contiguous stack geometrically, amortising
relocation cost over the amount of storage allocated.

Because growth can relocate the stack, stack-resident values must be
trivially relocatable (RFC-11). Values requiring stable addresses can be
heap-allocated, with handles stored on the stack.

One potential alternative is to have `frame_layout` specify a hard upper
limit on each stack frame's capacity and allocate each frame separately.
This would remove the trivial-relocatability requirement for
stack-resident types, at the cost of additional allocations and
potentially higher memory usage. We have not yet evaluated this approach
in practice.

## 5. Validation

The `vm` relies on the assembler, compiler, or loader to validate code
before execution. Its own invariant checks use `assert()` and are compiled
out when `NDEBUG` is defined.

Instruction implementations may perform additional runtime checks as
required by their semantics.

## 6. Instruction fusion

Instructions frequently executed in sequence could be combined into a new
instruction, reducing dispatch overhead at the cost of additional variants
and code size.

Candidate sequences should come from profiles of representative programs.
No specific fusion is proposed here.

## 7. Potential next steps

- Benchmark representative programs running on the VM and identify common
  bottlenecks.
- Identify frequently accessed values (such as the stack top) that could
  be passed as additional `execute_fn` arguments, following the approach
  used for `ip`. Measure the effects on loads, stores, and register pressure.
- Investigate whether storing frequently accessed values in thread-local
  variables, alongside or instead of passing them as `execute_fn` arguments,
  can improve performance.
