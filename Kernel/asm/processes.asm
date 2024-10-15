GLOBAL createProcessStack
GLOBAL start

start:
    call rdx
    mov rdi, 0x1E ; code designated as exit function
    int 80h

createProcessStack:
    push rbp
    mov rbp, rsp
    
    mov rsp, rdx ; Set up new stack pointer
    mov rbp, rdx ; Set up new base pointer

    push 0x0    ; SS 
    push rdx    ; RSP
    push 0x202  ; RFLAGS
    push 0x8    ; CS 
    push rcx    ; RIP (start)

    ; Parameters already in the correct registers:
    ; rdi: argc
    ; rsi: argv
    ; rdx: rsp
    ; rcx: start
    pushState  

    mov rax, rsp ; Return new stack pointer
    mov rsp, rbp
    pop rbp
    ret 