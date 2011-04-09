#ifndef TASK_H
#define TASK_H

#include "stddef.h"
#include "stdbool.h"
#include "fs/vfs.h"

#define MAX_TASKS 128
#define MAX_FDS 64

typedef struct tss
{	
	ushort link;
	ushort _res_1;
	
	uint esp0;
	
	ushort ss0;
	ushort _res_2;
	
	uint esp1;
	
	ushort ss1;
	ushort _res_3;
	
	uint esp2;
	
	ushort ss2;
	ushort _res_4;
	
	uint cr3;
	uint eip;
	uint eflags;
	uint eax;
	uint ecx;
	uint edx;
	uint ebx;
	uint esp;
	uint ebp;
	uint esi;
	uint edi;
	
	ushort es;
	ushort _res_5;
	
	ushort cs;
	ushort _res_6;
	
	ushort ss;
	ushort _res_7;
	
	ushort ds;
	ushort _res_8;
	
	ushort fs;
	ushort _res_9;
	
	ushort gs;
	ushort _res_10;
	
	ushort ldtr;
	ushort _res_11;
	
	ushort _res_12;
	ushort iopb;
}
__attribute__((__packed__)) tss_t;

typedef enum task_state
{	
	SLEEPING,
	RUNNING,
	IO_WAIT,
	KILLED,
}
task_state_t;

typedef struct task
{		
	// must be first member, so the offsets of members are known.
	tss_t tss;
	uint pid;
	uint* page_directory;
	task_state_t state;
	char kernel_stack[8192];
	vfs_stream_t* fds[MAX_FDS];
	uint exception_handler;
}
__attribute__((__packed__)) /* so i can access the struct straight from assembly */ task_t;

typedef struct regs
{	
	uint es;
	uint fs;
	uint gs;
	uint ds;
	uint edi;
	uint esi;
	uint ebp;
	uint esp;
	uint ebx;
	uint edx;
	uint ecx;
	uint eax;
}
__attribute__((__packed__)) regs_t;

typedef struct interrupt_stack
{
	uint ss;
	uint esp;
	uint eflags;
	uint cs;
	uint eip;
}
__attribute__((__packed__)) interrupt_stack_t;

extern uint current_task;
extern uint multitasking_enabled;

task_t* task_get(uint pid);
void task_init(uint base_physical, uint high_memory);
uint alloc_page();
void free_page(uint page);
task_t* task_create(uint size, void* code, uint stack_size);
task_t* task_dup(task_t* task);
void task_kill_and_free(task_t* task);
uint task_add_page(task_t* task);
void task_del_page(task_t* task, uint virtual);
void task_add_virtual(task_t*, uint virtual);
void task_switch();

task_t* task_current();

void task_peek(task_t* task, uint virtual, uint length, void* buffer);

#endif
