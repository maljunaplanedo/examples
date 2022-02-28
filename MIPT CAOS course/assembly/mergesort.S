.intel_syntax noprefix

// rcx n
// rdx in
// r13 out
// rsi len
// rdi i
// r9  jl
// r10 jr
// r11 mid
// r12 end

// eax left_val / temp32
// ebx right_val


.global mergesort
mergesort:                      // (from -> rdi, to -> rsi, in -> rdx, out -> rcx)
    sub rsi, rdi                // @arg_to -= @arg_from;
    imul rdi, 4                 // @arg_from *= 4
    add rdx, rdi                // @arg_in += @arg_from;

                                // in = @arg_in;
    mov r13, rcx                // out = @arg_out;
    mov rcx, rsi                // n = @arg_to;

    mov rdi, 0                  // i = 0;
LoopCopyBegin:
    mov eax, [rdx + rdi * 4]    // temp32 = in[i];
    mov [r13 + rdi * 4], eax    // out[i] = temp32;

    add rdi, 1                  // i++
    cmp rdi, rcx                // if (i == n)
    jz LoopCopyEnd              // then break LoopCopy;
    jmp LoopCopyBegin           // else continue LoopCopy;
LoopCopyEnd:

    mov rsi, 1                  // len = 1;
LoopLenBegin:
    mov rdi, 0;                 // i = 0;
LoopIBegin:

    mov r11, rdi                // mid = i
    add r11, rsi                // mid += len

    cmp r11, rcx                // if (mid <= n)
    jle LMidNoOverflow          // then goto LMidNoOverflow;
    mov r11, rcx                // else mid = n;
LMidNoOverflow:
    
    mov r12, r11                // end = mid;
    add r12, rsi                // end += len;

    cmp r12, rcx                // if (end <= n)
    jbe LEndNoOverflow          // then goto LEndNoOverflow;
    mov r12, rcx                // else end = n;
LEndNoOverflow:

    mov r9, rdi                 // jl = i;
    mov r10, r11                // jr = mid;
LoopMergeBegin:
    cmp r9, r11                 // if (jl == mid)
    jz LLeftAtEnd               // then goto LLeftAtEnd;
    
    cmp r10, r12                // if (jr == end)
    jz LRightAtEndOnly          // then goto LRightAtEndOnly;

    mov eax, [r13 + r9 * 4]     // left_val = out[jl];
    mov ebx, [r13 + r10 * 4]    // right_val = out[jr];

    cmp eax, ebx                // if (left_val < right_val)
    jl LLeftSmaller             // then goto LLeftSmaller

    add r10, 1                  // else jr++,
    sub rsp, 4                  // stack_ptr -= 4
    mov [rsp], ebx              // *stack_ptr = right_val
    jmp LEndMergeIf             // goto LEndMergeIf;

LLeftSmaller:
    add r9, 1                   // jl++,
    sub rsp, 4                  // stack_ptr -= 4
    mov [rsp], eax              // *stack_ptr = left_val
    jmp LEndMergeIf             // goto LEndMergeIf;

LLeftAtEnd:
    cmp r10, r12                // if (jr == end)
    jz LBothAtEnd               // then goto LBothAtEnd;

    mov eax, [r13 + r10 * 4]    // else tmp32 = out[jr],
    add r10, 1                  // jr++,
    sub rsp, 4                  // stack_ptr -= 4
    mov [rsp], eax              // *stack_ptr = tmp32
    jmp LEndMergeIf             // goto LEndMergeIf;

LRightAtEndOnly:
    mov eax, [r13 + r9 * 4]     // tmp32 = out[jl];
    add r9, 1                   // jl++;
    sub rsp, 4                  // stack_ptr -= 4;
    mov [rsp], eax              // *stack_ptr = tmp32
    jmp LEndMergeIf             // goto LEndMergeIf;

LBothAtEnd:
    jmp LoopMergeEnd            // break LoopMerge

LEndMergeIf:
    jmp LoopMergeBegin          // continue LoopMerge; 

LoopMergeEnd:
    
    mov r10, r12                // jr = end;
    add r10, -1                 // jr--;
    
LoopSetRBegin:
    cmp r10, r11                // if (jr < mid)
    jl LoopSetREnd              // break LoopSetR;
    
    mov eax, [rsp]              // tmp32 = *stack_ptr;
    add rsp, 4                  // stack_ptr += 4;
    mov [r13 + r10 * 4], eax    // out[jr] = tmp32;

    add r10, -1                 // jr--;
    jmp LoopSetRBegin           // continue LoopSetR;
LoopSetREnd:

    mov r9, r11                 // jl = mid;
    add r9, -1                  // jl--;

LoopSetLBegin:
    cmp r9, rdi                 // if (jl < i)
    jl LoopSetLEnd              // break LoopSetL;
    
    mov eax, [rsp]              // tmp32 = *stack_ptr;
    add rsp, 4                  // stack_ptr += 4;
    mov [r13 + r9 * 4], eax     // out[jl] = tmp32

    add r9, -1                  // jl--;
    jmp LoopSetLBegin           // continue LoopSetL;
LoopSetLEnd:
    
    add rdi, rsi                // i += len;
    add rdi, rsi                // i += len;
    cmp rdi, rcx                // if (i >= n)
    jge LoopIEnd                // then break LoopI
    jmp LoopIBegin              // continue LoopI

LoopIEnd:

    imul rsi, 2                 // len *= 2;
    cmp rsi, rcx                // if (len > n)
    jg LoopLenEnd               // then break LoopLen;
    jmp LoopLenBegin            // continue LoopLen;

LoopLenEnd:
    ret                         // return;

