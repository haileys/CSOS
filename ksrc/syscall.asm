global syscall_isr
extern multitasking_enabled
extern syscall_handler
extern page_directory

syscall_isr:
	cli
	mov [.saved_esp], esp
	pusha
	mov eax, 0
	mov [multitasking_enabled], eax
	
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
	
	push dword [.saved_esp]
	push esp
	
	call syscall_handler
	add esp, 4
	
	pop es
	pop fs
	pop gs
	pop ds	
	
	mov eax, [.saved_cr3]
	mov cr3, eax
	
	mov eax, 1
	mov [multitasking_enabled], eax
	popa
	iret
	.saved_cr3 dd 0
	.saved_esp dd 0
