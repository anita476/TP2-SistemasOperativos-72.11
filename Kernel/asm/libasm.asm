GLOBAL cpu_vendor
GLOBAL get_key
GLOBAL get_seconds
GLOBAL get_minutes
GLOBAL get_hours
GLOBAL inb
GLOBAL outb

section .text
	
cpu_vendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid

	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

get_key: 
	push rbp
	mov rbp, rsp

	in al, 0x60

	mov rsp, rbp
	pop rbp
	ret 

get_seconds:   
    cli

    ; Binary number configuration
    mov al, 0x0B 
 	out 70h, al
    in al, 71h
    or al, 4 
    out 71h, al 

    ; Seconds read
    mov al, 0x00
    out 70h, al
    in al, 71h

    sti
    ret
            
get_minutes:
    cli

    ; Binary number configuration
    mov al, 0x0B 
    out 70h, al
    in al, 71h
    or al, 4 
    out 71h, al 

    ; Minutes read
    mov al, 0x02
    out 70h, al
    in al, 71h

    sti
    ret

get_hours:
    cli

    ; Binary number configuration
    mov al, 0x0B 
    out 70h, al
    in al, 71h
	or al, 4 
    out 71h, al 

    ; Hours read
    mov al, 0x04
    out 70h, al
    in al, 71h
    
	sti
    ret

; Code segment inspired from: https://wiki.osdev.org/PC_Speaker
inb:
	push rbp
	mov rbp, rsp
	mov rdx, rdi
	mov rax, 0
    in al, dx
	mov rsp, rbp
	pop rbp
	ret

outb:
	push rbp
	mov rbp, rsp
	mov rax, rsi
	mov rdx, rdi
	out dx, al
	mov rsp, rbp
	pop rbp
	ret