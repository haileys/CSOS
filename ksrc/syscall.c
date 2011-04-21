#include "syscall.h"
#include "console.h"
#include "task.h"
#include "panic.h"
#include "fs/vfs.h"
#include "paging.h"
#include "idt.h"

static void(*syscall_vectors[])() = {
	// 0x00
	NULL,
	sys_alloc_page,
	sys_free_page,
	sys_exit,
	// 0x04
	sys_kill,
	sys_get_pid,
	sys_fork,
	sys_write,
	// 0x08
	sys_read,
	sys_open,
	sys_close,
	sys_write_ebx,
	// 0x0C
	sys_exception_handler,
};
static uint syscall_max = sizeof(syscall_vectors) / sizeof(*syscall_vectors);

void syscall_handler(regs_t* registers)
{
	if(registers->eax >= syscall_max)
	{
		kprintf("Unknown syscall %x\n", registers->eax);
		return;
	}
	kprintf("[syscall] %x\n", registers->eax);
	syscall_vectors[registers->eax](registers);
}

void sys_alloc_page(__attribute__((unused)) regs_t* registers)
{
	uint page = alloc_page();
	task_t* t = task_current();
	uint d_ent = 0;
	uint* tbl = 0;
	uint dir;
	for(dir = 0x10000000 / 4096 / 1024; dir < 0xffffffff / 4096 / 1024; dir++)
	{
		if(t->page_directory[dir] & 1)
			d_ent = t->page_directory[dir] & 0xfffff000;
		else
			continue;
	}
	tbl = (uint*)d_ent;
	uint tbl_i;
	for(tbl_i = 0; tbl_i < 1024; tbl_i++)
	{
		if((tbl[tbl_i] & 1) == 0)
			break;
	}
	if(tbl_i == 1024)
	{
		dir++;
		// fuck around and shit
	}
	tbl[tbl_i] = page | 1 | 2 | 4;
	registers->eax = ((dir * 1024) + page) * 4096;
}
void sys_free_page(__attribute__((unused)) regs_t* registers)
{
	
}
extern uint current_task;
extern uint multitasking_enabled;
void sys_exit(__attribute__((unused)) regs_t* registers)
{	
	task_kill_and_free(task_get(current_task));
	current_task = 0xffffffff;
	sti();
	multitasking_enabled = true;
	for(;;) ;
}
void sys_kill(__attribute__((unused)) regs_t* registers)
{
	if(registers->ebx >= MAX_TASKS || task_get(registers->ebx) == NULL)
	{
		registers->eax = 0;
		return;
	}
	if(registers->ebx == task_current()->pid)
	{
		sys_exit(registers);
	}
	task_kill_and_free(task_get(registers->ebx));
}
void sys_get_pid(regs_t* registers)
{
	registers->eax = task_current()->pid;
}
void sys_fork(__attribute__((unused)) regs_t* registers)
{
	task_t* new = task_dup(task_current());

	if(new == NULL)
	{
		registers->eax = 0xffffffff;
		return;
	}

	// the really tricky part about this is that the new task won't be returning from a syscall.
	// it'll be spun up by the switcher so we need to 'fix' the return EIP so that it is switched into properly.
	// that means we copy the registers from our current registers and modify EAX to be the new pid.
	
	new->tss.ss = *(uint*)(task_current()->tss.esp0 - 1*4);
	new->tss.esp = *(uint*)(task_current()->tss.esp0 - 2*4);
	new->tss.eflags = *(uint*)(task_current()->tss.esp0 - 3*4);
	new->tss.cs = *(uint*)(task_current()->tss.esp0 - 4*4);
	new->tss.eip = *(uint*)(task_current()->tss.esp0 - 5*4);
	
	// now set return values:
	registers->eax = new->pid;
	new->tss.eax = new->pid;
	
	// all this magic should work in theory, but I haven't tested it yet.

}
void sys_write(regs_t* registers)
{
	if(registers->ebx >= MAX_FDS || task_current()->fds[registers->ebx] == NULL)
	{
		registers->eax = 0;
		return;
	}
	vfs_stream_t* stream = task_current()->fds[registers->ebx];
	for(uint vaddr = registers->ecx; vaddr < registers->ecx + registers->edx;)
	{
		uint len = 4096 - vaddr % 4096;
		if((registers->ecx + registers->edx) - vaddr < len)
			len = (registers->ecx + registers->edx) - vaddr;
		uint phys = (uint)paging_virtual_to_physical(task_current()->page_directory, vaddr);
		stream->write(stream->state, len, (void*)phys);
		vaddr += len;
	}
	registers->eax = registers->edx;
}
void sys_read(__attribute__((unused)) regs_t* registers)
{
	
}
void sys_open(__attribute__((unused)) regs_t* registers)
{
	
}
void sys_close(__attribute__((unused)) regs_t* registers)
{
	
}
void sys_write_ebx(regs_t* registers)
{
	kprintf("EBX = 0x%x\n", registers->ebx);
}
void sys_exception_handler(regs_t* registers)
{
	task_current()->exception_handler = registers->ebx;
}
