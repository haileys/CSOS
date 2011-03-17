extern memcpy
extern panic
global get_gdt
global gdt_reload_segment_registers

get_gdt:
	sgdt [.tmp]
	mov eax, .tmp
	ret
	.tmp	dd 0
			dd 0
			
gdt_reload_segment_registers:
	jmp 0x08:.boom
	.boom:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret