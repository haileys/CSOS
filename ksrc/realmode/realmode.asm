use32
bits 32
org 0x8000

entry:
	pusha
	mov [saved_esp], esp
	mov eax, 0x4000
	mov esp, eax
	jmp 0x18:move_to_real

move_to_real:
	use16
	mov eax, 0x20
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax

	mov eax, cr0
	and al, ~1
	mov cr0, eax

	jmp 0:in_real

in_real:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov sp, 0x4000
	
	lidt [idt_real]
	sti
	
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	xor esi, esi
	xor edi, edi
	
	call 0x9000
	
	cli
	mov eax, cr0
	or al, 1
	mov cr0, eax
	lidt [idt_prot]
	sti
	
	jmp 0x08:protected
	
protected:
	use32
	mov eax, 0x10
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	mov esp, [saved_esp]
	popa
	ret

saved_esp:
	dd 0
idt_real:
	dw 0x3ff
	dd 0
gdt_prot:
	dw 0x1000
	dd 0x00000600
idt_prot:
	dw (256*8)-1
	dd 0x00001600