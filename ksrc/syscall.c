#include "syscall.h"
#include "console.h"
#include "task.h"
#include "panic.h"
#include "fs/vfs.h"
#include "paging.h"

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
};
static uint syscall_max = sizeof(syscall_vectors) / sizeof(*syscall_vectors);

void syscall_handler(regs_t* registers)
{
	if(registers->eax >= syscall_max)
	{
		kprintf("Unknown syscall %x\n", registers->eax);
		return;
	}
	syscall_vectors[registers->eax](registers);
}

void sys_alloc_page(__attribute__((unused)) regs_t* registers)
{
	
}
void sys_free_page(__attribute__((unused)) regs_t* registers)
{
	
}
extern uint current_task;
void sys_exit(__attribute__((unused)) regs_t* registers)
{
	// DANGER DANGER DANGER
	// when we free this task, the stack we are on is no longer valid.
	// we can still use it of course, but we don't have the memory region to ourselves anymore.
	// we'll temporarily disable interrupts and GTFO
	cli();
	task_kill_and_free(task_current());
	current_task = 0xffffffff; // magic value that indicates to the switcher that there is no valid task running.
	
	__asm__("int 32"); // trigger a task switch
}
void sys_kill(__attribute__((unused)) regs_t* registers)
{
	
}
void sys_get_pid(regs_t* registers)
{
	registers->eax = task_current()->pid;
}
void sys_fork(__attribute__((unused)) regs_t* registers)
{
	
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