use32
global panic
global symtable
global get_symtable_addr
extern kprintf
extern panic_backtrace_step

section .text

panic:
	push ss
	push ds
	push cs
	push ebx
	push edx
	push ecx
	push eax
	push dword [esp+(8*4)]
	push .msg
	call kprintf
	add esp, 9*4
	
	push ebp
	mov ebp, esp
	
	.lp:
		mov esp, ebp
		pop ebp
		pop ebx
		mov [.tmp], ebx
		push ebx
		call panic_backtrace_step
		add esp, 4
		mov eax, [.tmp]
		cmp eax, 0
		jne .lp
	cli
	hlt
	.tmp dd 0
	.msg db 10, "Kernel Panic - %s", 10, "EAX: %x  ECX: %x  EDX: %x  EBX: %x  CS: %x  DS: %x  SS: %x", 10, 0

get_symtable_addr:
	mov eax, symtable
	ret

section .data
symtable:
	; magic markers
	dd 0xffffffff
	dd 0xdeadbeef
	dd 0xbadf00de
	dd 0xffffffff

	times 65536 db 0 ; reserve 64KiB for the symbol table
