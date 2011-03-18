#ifndef TASK_H
#define TASK_H

#include "stddef.h"
#include "stdbool.h"

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
}
task_state_t;

typedef struct task
{
	tss_t tss;
	uint page_directory[1024] __attribute__((aligned(0x1000)));
	task_state_t state;
}
task_t;

void task_init(uint base_physical, uint high_memory);
uint alloc_page();
void free_page();

#endif