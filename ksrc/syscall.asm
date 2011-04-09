global syscall_isr
extern multitasking_enabled
extern syscall_handler
extern page_directory

syscall_isr:
	cli
	pusha
	
	mov eax, cr3
	mov [.saved_cr3], eax
	mov eax, page_directory
	mov cr3, eax
	
	push ds
	push gs
	push fs
	push es
	
	mov eax, 0x10
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
	
	mov eax, [.saved_cr3]
	mov cr3, eax
	
	popa
	sti
	iret
	.saved_cr3 dd 0