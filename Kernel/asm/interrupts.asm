GLOBAL _cli
GLOBAL _sti
GLOBAL _hlt
GLOBAL halt_cpu
GLOBAL int81
GLOBAL _schedule

GLOBAL pic_master_mask
GLOBAL pic_slave_mask

GLOBAL _irq00_handler
GLOBAL _irq01_handler
GLOBAL _int80_handler

GLOBAL _exception0_handler
GLOBAL _exception6_handler

GLOBAL show_registers_dump
GLOBAL has_regs
GLOBAL show_registers
EXTERN exception_dispatcher
EXTERN syscall_handler
EXTERN irq_dispatcher

EXTERN print
EXTERN switch_process

SECTION .text

%macro push_state 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push rax

%endmacro

%macro pop_state 0
	pop rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro irqHandlerMaster 1
	push_state

	mov rdi, %1 
	call irq_dispatcher

	; Signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	pop_state
	iretq
%endmacro

%macro  exception_handler 1
	; Save registers in data 
	; Order: RIP RAX RBX RCX RDX RSI RDI RBP RSP R8 R9 R10 R11 R12 R13 R14 R15 
    mov [show_registers + (1*8)] , rax
    mov rax, [rsp]
    mov [show_registers], rax ; rip
    mov [show_registers + (2*8)], rbx
    mov [show_registers + (3*8)], rcx
    mov [show_registers + (4*8)], rdx
    mov [show_registers + (5*8)], rsi 
    mov [show_registers + (6*8)], rdi
    mov [show_registers + (7*8)], rbp

    ; RSP from stack (flags, CS and RIP where pushed when int occurs)
    mov rax, [rsp + 3*8]
    mov [show_registers + (8*8)], rax
    
    mov [show_registers + (9*8)], r8
    mov [show_registers + (10*8)], r9
    mov [show_registers + (11*8)], r10
    mov [show_registers + (12*8)], r11
    mov [show_registers + (13*8)], r12
    mov [show_registers + (14*8)], r13
    mov [show_registers + (15*8)], r14
    mov [show_registers + (16*8)], r15
    mov rax, [rsp+16] ; RFLAGS (in stack bc interruption happened).
    mov [show_registers + (17*8)], rax    ; rflags

    ; Load parameters to pass to exceptions handler
    mov rdi, %1 
    mov rsi, show_registers ; "pointer to string"
    call exception_dispatcher

    iretq
%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret

_sti:
	sti
	ret

pic_master_mask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h, al
    pop rbp
    retn

pic_slave_mask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	0A1h, al
    pop rbp
    retn

; 8254 Timer (Timer Tick)
_irq00_handler:
	push_state

	mov rdi, 0
	call irq_dispatcher
	
	mov rdi, rsp
	call switch_process
	mov rsp, rax

	mov al, 20h
	out 20h, al
	
	pop_state
	iretq

; Keyboard
_irq01_handler:
	push_state
    in al, 0x60 ; readKey
    cmp al, 0x1D ; check if left CTRL is pressed (used to save registers)
    jne .continue
        ; mov [show_registers_dump],         rax -> dont do this, it only gets 1D from ctrl key
        mov [show_registers_dump + (1*8)], rbx 
        mov [show_registers_dump + (2*8)], rcx
        mov [show_registers_dump + (3*8)], rdx 
        mov [show_registers_dump + (4*8)], rsi 
        mov [show_registers_dump + (5*8)], rdi
        mov [show_registers_dump + (6*8)], rbp 
        mov [show_registers_dump + (8*8)], r8
        mov [show_registers_dump + (9*8)], r9
        mov [show_registers_dump + (10*8)], r10
        mov [show_registers_dump + (11*8)], r11
        mov [show_registers_dump + (12*8)], r12
        mov [show_registers_dump + (13*8)], r13
        mov [show_registers_dump + (14*8)], r14
        mov [show_registers_dump + (15*8)], r15 

        ; RSP && RIP && RAX
        mov rax , [rsp + 18*8] 
        mov [show_registers_dump + (7*8)], rax
        mov rax, [rsp + 15*8]
        mov [show_registers_dump + (16*8)], rax
        mov rax , [rsp + 14*8]
        mov [show_registers_dump], rax
        mov byte[has_regs], 1

.continue:
    mov rdi, 1 ; param for dispatcher
    call irq_dispatcher
        
    ; EOI
    mov al, 20h
    out 20h, al

    pop_state
    iretq

; Zero Division Exception
_exception0_handler:
	exception_handler 0

; Invalid Opcode Exception
_exception6_handler:
	exception_handler 6

_int80_handler:
  	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	; mov rdi, rax
    call syscall_handler
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	iretq

halt_cpu:
	cli
	hlt
	ret

int81:
	int 81h
	ret

_schedule: 
	push_state
	
	mov rdi, rsp
	;mov rsp, rdi
	call switch_process
	mov rsp, rax

	pop_state
	iretq

SECTION .bss
    has_regs resb 1 ; to check whether we have saved or not!
    show_registers resq 18 ; reserve a qword for each register 
    show_registers_dump resq 17 ; aditionally for dumping (isnt passed as a param but is accessed directly)