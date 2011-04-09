extern main

global alloc_page	; vec 0x01
global free_page	; vec 0x02
global exit			; vec 0x03
global kill			; vec 0x04

global get_pid		; vec 0x05
global fork			; vec 0x06
global write		; vec 0x07
global read			; vec 0x08

global open			; vec 0x09
global close		; vec 0x0a

call main
call exit
jmp $

alloc_page:
	mov eax, 0x01
	int 0x80
	ret
	
free_page:
	push ebx
	mov eax, 0x02
	mov ebx, [esp+8]
	int 0x80
	pop ebx
	ret
	
exit:
	mov eax, 0x03
	int 0x80
	ret
	
kill:
	push ebx
	mov eax, 0x04
	mov ebx, [esp+8]
	int 0x80
	pop ebx
	ret
	
get_pid:
	mov eax, 0x05
	int 0x80
	ret

fork:
	mov eax, 0x06
	int 0x80
	ret
	
write:
	push ebx
	mov eax, 0x07
	mov ebx, [esp+8]
	mov ecx, [esp+12]
	mov edx, [esp+16]
	int 0x80
	pop ebx
	ret
	
read:
	push ebx
	mov eax, 0x08
	mov ebx, [esp+8]
	mov ecx, [esp+12]
	mov edx, [esp+16]
	int 0x80
	pop ebx
	ret
	
open:
	push ebx
	mov eax, 0x09
	mov ebx, [esp+8]
	int 0x80
	pop ebx
	ret
	
close:
	push ebx
	mov eax, 0x0a
	mov ebx, [esp+8]
	int 0x80
	pop ebx
	ret
	