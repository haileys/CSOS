extern main

;;;; syscalls

global alloc_page			; vec 0x01
global free_page			; vec 0x02
global _PDCLIB_Exit			; vec 0x03
global kill					; vec 0x04

global get_pid				; vec 0x05
global fork					; vec 0x06
global write				; vec 0x07
global read					; vec 0x08

global open					; vec 0x09
global close				; vec 0x0a

global exception_handler	; vec 0x0c

;;;; support for pdclib
global stdin
global stdout
global stderr
global __divdi3
global errno
global __errno_location

push __raw_exception_handler
call exception_handler
add esp, 4

call main
push eax
call _PDCLIB_Exit
jmp $

stdin dd 0
stdout dd 1
stderr dd 1 ; @TODO fix this to have stderr be its own fd
errno dd 0
__errno_location dd 0

__raw_exception_handler:
	push ebx
	call __c_exception_handler
	add esp, 4
	ret

__c_exception_handler:
	push ebp
	mov ebp, esp
	
	push dword 21
	push .msg
	push dword 1
	call write
	add esp, 12
	
	push dword 1
	call _PDCLIB_Exit

section .data
	align 16
	.msg db "Unhandled Exception.", 0

section .text

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
	
_PDCLIB_Exit:
	mov ebx, [esp+4]
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

exception_handler:
	push ebx
	mov eax, 0x0c
	mov ebx, [esp+8]
	int 0x80
	pop ebx
	ret	

