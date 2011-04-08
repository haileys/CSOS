global syscall_isr
extern multitasking_enabled
extern syscall_handler

syscall_isr:
	cli
	pusha
	push ds
	push gs
	push fs
	push es
	
	mov ax, 0x10
	mov ds, ax
	mov gs, ax
	mov fs, ax
	mov es, ax
	
	push esp
	call syscall_handler
	add esp, 4
	
	pop es
	pop fs
	pop gs
	pop ds	
	popa	
	sti
	iret