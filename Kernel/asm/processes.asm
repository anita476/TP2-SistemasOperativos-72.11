GLOBAL createProcessStack
GLOBAL start

start:
    call rdx
    mov rdi, 0x1E ; code designated as exit function
    int 80h

createProcessStack:
    push rbp
    mov rbp, rsp
    
    mov rsp, rdx
    push 0x0
    push rdx
    push 0x202
    push 0x8
    push start   
    
    push 0x01
    push 0x02
    push rcx
    push rdx
	push rdi
	push rsi
	push 0x08
	push 0x09
	push 0x0A
	push 0x0B
	push 0x0C
	push 0x0D
	push 0x0E
	push 0x0F   
	push 0x10

    mov rax, rsp ; rsp that a process structure saves
    mov rsp, rbp
    pop rbp
    ret 