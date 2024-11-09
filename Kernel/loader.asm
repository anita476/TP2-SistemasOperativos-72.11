GLOBAL loader
GLOBAL reset_main
extern main
extern initialize_kernel_binary
extern get_stack_base
section .text

loader:
	call initialize_kernel_binary	; Set up the kernel binary, and get thet stack address
	jmp continue
reset_main: ; to reset after exceptions
	call get_stack_base
continue:
	mov rsp, rax				; Set up the stack with the returned address
	call main
hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang
