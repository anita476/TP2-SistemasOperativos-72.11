GLOBAL syscall
GLOBAL div_zero
GLOBAL invalid_opcode
section .text
GLOBAL halt_cpu

div_zero:
	mov rax, 5
	mov ebx, 0
	div ebx
	ret

invalid_opcode:
    mov cr6, rax
    ret

syscall:	
	push rbp;
	mov rbp, rsp;
	int 80h

	mov rsp, rbp;
	pop rbp;
	ret	

halt_cpu:
	cli
	hlt
	ret