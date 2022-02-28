.intel_syntax noprefix
.text
.global dot_product
// N rdi, A rsi, B rdx
// rbx = 8 * (N div 8)   (n)
// ymm0 result
// rax i
// ymm1 A[i], product
// ymm2 B[i], 0
// ecx tail_val
// rcx 32*n
dot_product:
    mov rbx, rdi // n = N 
    and rbx, 0xfffffffffffffff8 // (n = n[64:2], 0, 0)
   
    vxorps ymm0, ymm0, ymm0 // result = 0

    cmp rbx, 0 // if n == 0
    jz LoopMainEnd // then no main loop

    mov rax, 0 // i = 0
LoopMain:
    vmovups ymm1, [rsi + rax * 4] // get A[i]
    vmovups ymm2, [rdx + rax * 4] // get B[i]

    vmulps ymm1, ymm1, ymm2 // product = A[i] * B[i]
    vaddps ymm0, ymm0, ymm1 // result += product

    add rax, 8 // i += 8
    cmp rax, rbx // if i < n
    jl LoopMain // continue
    
    mov rcx, rbx // rcx = n
    imul rcx, 4 // rcx *= 32

    add rsi, rcx // A += 32*n
    add rdx, rcx // B += 32*n
    sub rdi, rbx // N -= n

    cmp rdi, 0
    jz LTailProcEnd
LoopMainEnd:

    sub rsp, 32 // stack_ptr <- 32 bytes
    vxorps ymm2, ymm2, ymm2 // ymm2 = 0
    vmovups [rsp], ymm2 // stack_ptr[0:256] = 0

    mov rax, 0 // i = 0
LoopTailA:
    mov ecx, [rsi + rax * 4] // ecx = A[i]
    mov [rsp + rax * 4], ecx // stack_ptr[i] = ecx
    
    inc rax // i++
    cmp rax, rdi // if i < N
    jl LoopTailA // continue

    vmovups ymm1, [rsp] // "A[i]" = *stack_ptr;

    vmovups [rsp], ymm2 // stack_ptr[0:256] = 0

    mov rax, 0 // i = 0
LoopTailB:
    mov ecx, [rdx + rax * 4] // ecx = B[i]
    mov [rsp + rax * 4], ecx // stack_ptr[i] = ecx

    inc rax // i++
    cmp rax, rdi // if i < N
    jl LoopTailB // continue

    vmovups ymm2, [rsp] // "B[i]" = *stack_ptr
    
    vmulps ymm1, ymm1, ymm2 // "A[i]" * "B[i]" -> ymm1
    vaddps ymm0, ymm0, ymm1 // result += ymm1

    add rsp, 32 // stack_ptr -> 32 bytes

LTailProcEnd:

    vhaddps ymm0, ymm0, ymm0 // sum of first 4 in 0th,
    vhaddps ymm0, ymm0, ymm0 // sum of last 4 in 4th

    vxorps ymm2, ymm2, ymm2 // ymm2 = 0
    vblendps ymm0, ymm2, ymm0, 0b00010001 // leave only 0th and 4th

    vextractf128 xmm1, ymm0, 1 // xmm1 = ymm0[128:256]
    vaddps xmm0, xmm0, xmm1 // xmm0 = ymm0[0:128] + ymm0[128:256]
   
    ret

