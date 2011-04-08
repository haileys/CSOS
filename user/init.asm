org 0x10000000
use32

mov eax, 0x88 | 3
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

mov eax, 0xdeadbeef
int 0x80

nop

jmp $