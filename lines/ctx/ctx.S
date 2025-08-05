#if (__APPLE__)
  #define FUNCTION_NAME(name) _##name
#else
  #define FUNCTION_NAME(name) name
#endif

.global FUNCTION_NAME(SetupContext)

.global FUNCTION_NAME(SwitchContext)


# SetupContext(stack, trampoline, arg)

FUNCTION_NAME(SetupContext):
    # Arguments
    # 1) %rdi - bottom of the stack
    # 2) %rsi - static trampoline
    # 3) %rdx - ITrampoline

    # Switch to stack
    movq %rsp, %r11 # tmp := rsp
    movq %rdi, %rsp # rsp := rdi

    # trampoline stack must be 16-byte aligned
    subq $64, %rsp
    andq $-16, %rsp
    addq $8, %rsp

    # 7-th argument for trampoline
    movq %rdx, 8(%rsp)

    # Prepare for context switch

    # 1) Return address for SwitchContext
    pushq %rsi

    # 2) Callee-saved registers
    pushq $0 # r15
    pushq $0 # r14
    pushq $0 # r13
    pushq $0 # r12
    pushq $0 # rbx
    pushq $0 # rbp

    # Return value for SetupContext
    movq %rsp, %rax

    # Restore callstack
    movq %r11, %rsp # rsp := tmp

    retq


# SwitchContext(from_rsp, to_rsp)

FUNCTION_NAME(SwitchContext):
    # SwitchContext frame created on top of the current stack

    # 1. Save current execution context to 'from'

    # 1.1 Save callee-saved registers on top of the current stack

    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12

    pushq %rbx
    pushq %rbp

    # Switch stacks

    # 1.2 Save current stack pointer to 'from' Context
    movq %rsp, (%rdi) # from_rsp := rsp

    # 2. Activate 'to' execution context

    # 2.1 Set stack pointer to target stack
    movq (%rsi), %rsp # rsp := to_rsp

    # 2.2 Restore and pop registers saved on target stack

    popq %rbp
    popq %rbx

    popq %r12
    popq %r13
    popq %r14
    popq %r15

    # Pop current SwitchContext frame from target stack

    retq

#ifndef __APPLE__
# Mark that we don't need executable stack.
.section .note.GNU-stack,"",@progbits
#endif
