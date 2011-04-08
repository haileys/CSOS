extern memcpy
extern panic
global get_gdt
global gdt_reload_segment_registers
global switch_to_user_mode

get_gdt:
	sgdt [.tmp]
	mov eax, .tmp
	ret
	.tmp	dd 0
			dd 0
			
gdt_reload_segment_registers:
	jmp 0x08:.boom
	.boom:
	mov eax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
	
switch_to_user_mode:
	cli
	mov eax, esp
	push 0x88 + 3
	push eax
	pushf
	pop eax
	or eax, 0x200
	push eax
	push dword 0x80 + 3
	push .user
	iret
	
	.user:
	mov eax, 0x88
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	int 0x80
	jmp $
	
	hlt