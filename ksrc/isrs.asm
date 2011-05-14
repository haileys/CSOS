global asm_isr_init
extern isr_dispatch
extern idt_register_handler
extern panic
extern panicf
extern kprintf

%define WITH_ERROR push eax

%macro isr 1-2 push dword 0

	jmp isr_%1.fin
	isr_%1:
		mov [.eax], eax
		mov ax, 0x10
		mov ds, ax
		mov eax, [esp]
		mov [.err], eax
		mov eax, [.eax]
		pusha
		mov eax, [.err]
		%2
		push dword %1
		jmp isr_main
	.eax dd 0
	.err dd 0
	.fin:
	push isr_%1
	push dword %1
	call idt_register_handler
	add esp, 8

%endmacro

isr_main:
	cli
	call isr_dispatch
	add esp, 8
	
;	cmp eax, 8
;	jl .dont_send_other_clearance
		mov al, 0xa0
		out 0x20, al
	.dont_send_other_clearance:
	mov al, 0x20
	out 0x20, al
	mov ax, 0x8b
	mov ds, ax
	popa
	sti
	iret

asm_isr_init:

	isr 0
	isr 1
	isr 2
	isr 3
	isr 4
	isr 5
	isr 6
	isr 7
	isr 8, WITH_ERROR
	isr 9

	isr 10, WITH_ERROR
	isr 11, WITH_ERROR
	isr 12, WITH_ERROR
	isr 13, WITH_ERROR
	isr 14, WITH_ERROR
	isr 15
	isr 16
	isr 17, WITH_ERROR
	isr 18
	isr 19

	isr 20
	isr 21
	isr 22
	isr 23
	isr 24
	isr 25
	isr 26
	isr 27
	isr 28
	isr 29

	isr 30
	isr 31
;;
;; interrupt 32 is not subscribed the normal way as it is the special interrupt to trigger a task switch.
	isr 32

	isr 33
	isr 34
	isr 35
	isr 36
	isr 37
	isr 38
	isr 39

	isr 40
	isr 41
	isr 42
	isr 43
	isr 44
	isr 45
	isr 46
	isr 47
	isr 48
	isr 49
	
	isr 0x80
	
	ret
