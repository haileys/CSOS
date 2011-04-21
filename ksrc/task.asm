extern panic
extern kprintf
extern current_task
extern multitasking_enabled
extern tasks
extern task_install_next_tss
extern task_find_next
global task_switch

%define TIME_SLICE 3

which_slice dd 0

task_switch:
	cli
	; temporarily save registers out of the way while we load up task to store to
	mov [.eax], eax
	mov [.ebx], ebx
	mov [.ecx], ecx
	mov [.edx], edx
	
	mov eax, multitasking_enabled
	cmp eax, 0
	jne .perform_switch
	
	sti
	iret
	
	mov eax, [which_slice]
	cmp eax, TIME_SLICE
	je .perform_switch
	
	inc eax
	mov [which_slice], eax
	sti
	iret
	
	.perform_switch:
	xor eax, eax
	mov [which_slice], eax
	
	mov eax, [current_task]
	; if current_task is 0xffffffff then that means that there is no current task and we shouldn't save back the registers
	; this can happen on system initialization, or when a process dies.
	cmp eax, 0xffffffff
	jne .perform_save
		; make eax zero so our next algo works properly
		mov eax, 0
		jmp .skip_save
	.perform_save:
	mov ebx, [tasks + eax * 4]
	; ebx is now a pointer to our task_t struct. first member of which is the tss
	mov eax, [.eax]
	mov [ebx + 0x28], eax
	mov eax, [.ecx]
	mov [ebx + 0x2c], eax
	mov eax, [.edx]
	mov [ebx + 0x30], eax
	mov eax, [.ebx]
	mov [ebx + 0x34], eax
	
	; return EIP is at [esp]
	mov eax, [esp]
	mov [ebx + 0x20], eax
	; return CS is [esp+4]
	mov eax, [esp + 4]
	mov [ebx + 0x4c], ax ; cs is only 16 bits
	; EFLAGS
	mov eax, [esp + 8]
	mov [ebx + 0x24], eax
	; ESP
	mov eax, [esp + 12]
	mov [ebx + 0x38], eax
	; SS
	mov eax, [esp + 16]
	mov [ebx + 0x50], ax
	
	; save rest of registers
;	mov eax, cr3
;	mov [ebx + 0x1c], eax
	mov [ebx + 0x3c], ebp
	mov [ebx + 0x40], esi
	mov [ebx + 0x44], edi
	mov [ebx + 0x48], es
	mov [ebx + 0x54], ds
	mov [ebx + 0x58], fs
	mov [ebx + 0x5c], gs
	
	.skip_save:
	
	; eax contains the task index of the task we'll switch to
	mov eax, [current_task]
	push eax
	call task_find_next
	add esp, 4
	mov [current_task], eax
	
	push eax
	call task_install_next_tss
	add esp, 4 ; balance stack, caller cleanup
	
	mov eax, [current_task]
	mov ebx, [tasks + eax * 4]
	
	; now load up the new task's registers
	xor eax, eax
	; SS:
	mov ax, [ebx + 0x50]
	push eax
	; ESP:
	mov eax, [ebx + 0x38]
	push eax
	; EFLAGS:
	mov eax, [ebx + 0x24]
	or eax, 0x200
	push eax
	; CS:
	xor eax, eax
	mov ax, [ebx + 0x4c]
	push eax
	; EIP:
	mov eax, [ebx + 0x20]
	push eax
	
	mov ebp, [ebx + 0x3c]
	mov esi, [ebx + 0x40]
	mov edi, [ebx + 0x44]
	mov eax, [ebx + 0x1c]
	mov cr3, eax
	mov ax, [ebx + 0x48]
	mov es, ax
	mov ax, [ebx + 0x54]
	mov ds, ax
	mov ax, [ebx + 0x58]
	mov fs, ax
	mov ax, [ebx + 0x5c]
	mov gs, ax
	
	mov al, 0x20
	out 0x20, al
	
	mov ax, 0x93
	ltr ax
	
	mov eax, [ebx + 0x28]
	mov ecx, [ebx + 0x2c]
	mov edx, [ebx + 0x30]
	mov ebx, [ebx + 0x34]
	
	iret
	.eax dd 0
	.ebx dd 0
	.ecx dd 0
	.edx dd 0
	.switchmsg db "switching to task %d", 10, 0
	.msg db "EIP: %x, CS: %x, EFLAGS: %x, ESP: %x, SS: %x", 0
	.ping db "ping!", 0
