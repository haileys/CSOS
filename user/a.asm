org 0x10000000
use32

syscall:
	mov eax, 0xdeadbeef
	int 0x80

jmp syscall