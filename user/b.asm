org 0x10000000
use32

mov eax, 0x88 | 3
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

syscall:
	mov eax, 0xbadf00d
	int 0x80

jmp syscall