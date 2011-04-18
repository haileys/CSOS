#include "task.h"
#include "stddef.h"
#include "kmalloc.h"
#include "panic.h"
#include "gdt.h"
#include "string.h"
#include "idt.h"
#include "paging.h"
#include "console.h"
#include "syscall.h"

#define MAX_TASKS 16

uint user_base;
uint page_base;
uint total_pages;

uchar* pagemap = NULL;

task_t* tasks[MAX_TASKS];
uint current_task = 0xffffffff; // this means no task
uint num_tasks = 0;

tss_t kernel_tss;

uint multitasking_enabled = 0;

task_t* task_current()
{
	return tasks[current_task];
}

void task_init(uint base_physical, uint high_memory)
{
	user_base = base_physical;
	page_base = base_physical / PAGE_SIZE;
	total_pages = (high_memory * 1024 - base_physical) / PAGE_SIZE;
	kprintf("Total pages: %d\n", total_pages);
	pagemap = (uchar*)kmalloc(total_pages / 8);
	memset(pagemap, 0, total_pages / 8);
	
	// setup GDT entries for tasks
	gdt_entry_t task_seg;
	gdt_raw_entry_t task_seg_raw;
	
	gdt_code_entry_factory(&task_seg, 0, 0xffffffff, 3, false, true);
	gdt_encode(&task_seg, &task_seg_raw);
	gdt_set_entry(0x80, &task_seg_raw);
	
	gdt_data_entry_factory(&task_seg, 0, 0xffffffff, 3, true);
	gdt_encode(&task_seg, &task_seg_raw);
	gdt_set_entry(0x88, &task_seg_raw);
	
	idt_register_handler(0x80, (uint)syscall_isr);
	idt_set_privilege(0x80, 3);
}

uint alloc_page()
{
	for(uint i = 0; i < total_pages / 8; i++)
	{
		if(pagemap[i] == 0xff)
			continue;
		
		for(uint j = 0; j < 8; j++)
		{
			if(pagemap[i] & (1 << j))
				continue;
			
			pagemap[i] |= 1 << j;
			return (i * 8 + j + page_base) * PAGE_SIZE;
		}
	}
	panic("No free pages");
	return 0; // never reached
}
void free_page(uint page)
{
	page /= PAGE_SIZE;
	uint pdiv = page/8, pmod = page%8;

	if(page < page_base || page >= total_pages + page_base)
		panic("Freeing non-existent page");
		
	if(((pagemap[pdiv] >> pmod) & 1) == 0)
		panic("Freeing freed page");
		
	pagemap[pdiv] &= ~(1 << pmod);
}

task_t* task_create(uint size, void* code, uint stack_size)
{
 	if(num_tasks == MAX_TASKS) {
 		kprintf("Failed to create a task");
		return NULL;
	}
		
	uint stack_pages = (stack_size + PAGE_SIZE-1) / PAGE_SIZE;
	uint codedata_pages = (size + PAGE_SIZE-1) / PAGE_SIZE;
	
	task_t* task = (task_t*)kmalloc(sizeof(task_t));
	memset(task, 0, sizeof(task_t));
	
	uint total_pages = stack_pages + codedata_pages;
	uint total_dir_entries = (total_pages + 1023) / 1024;
	task->tss.cr3 = (uint)(task->page_directory = (uint*)alloc_page());
	memset(task->page_directory, 0, PAGE_SIZE);
	memcpy(task->page_directory, page_directory, 4 /* copy only first page directory (4MB) of ram. this is what the kernel will be using */);
	for(uint i = 0; i < total_dir_entries; i++)
	{
		task->page_directory[((0x10000000 / PAGE_SIZE) / 1024) + i] = alloc_page() | 1 | 2 | 4;
	}
	for(uint i = 0; i < total_pages; i++)
	{
		uint dir = i / 1024;
		uint* table = (uint*)(task->page_directory[dir + ((0x10000000 / PAGE_SIZE) / 1024)] & 0xfffff000);
		uint phys = alloc_page();
		table[i % 1024] = phys | 1 | 2 | 4;
		if(i * PAGE_SIZE < size)
			memcpy((void*)phys, (char*)code + (PAGE_SIZE * i), PAGE_SIZE);
	}
	
	task->state = RUNNING;
	task->tss.ss0 = 0x10; // kernel stack segment
	task->tss.esp0 = (uint)task->kernel_stack + 8192 - 4;
	task->tss.iopb = 0; // don't let it I/O anywhere
	
	task->tss.cs = 0x80 | 3;
	task->tss.ds = task->tss.ss = task->tss.es = task->tss.fs = task->tss.gs = 0x88 | 3;
	__asm__("pushf\npop eax" : "=a"(task->tss.eflags));
	task->tss.eflags |= 0x200; // interrupts enabled
	task->tss.eip = 0x10000000;
	
	task->tss.esp = 0x10000000 + total_pages * PAGE_SIZE - 4;
	
	// find free pid
	uint pid = 0;
	for(; pid < MAX_TASKS && tasks[pid] != NULL; pid++) ;
	num_tasks++;
	kprintf("New task with pid %d\n", pid);
	task->pid = pid;
	tasks[pid] = task;
	
	memset(task->fds, 0, sizeof(vfs_stream_t*) * MAX_FDS);
	task->fds[1 /* stdout */] = console_stdout();
	
	return task;
}

void task_kill_and_free(task_t* task)
{
	task->state = KILLED;
	for(uint i = 0; i < MAX_FDS; i++)
	{
		if(task->fds[i])
			task->fds[i]->close(task->fds[i]->state);
	}
	if(task == task_current())
	{
		__asm__("mov cr3, eax" :: "a"(&page_directory));
	}
	// work through page directory starting at 0x10000000 freeing pages
	for(uint dir_i = 0x10000000 / PAGE_SIZE / 1024; dir_i < 1024; dir_i++)
	{
		if(!(task->page_directory[dir_i] | 1))
		{
			// page directory entry not in use? skip
			continue;
		}
		uint* table = (uint*)(task->page_directory[dir_i] | 0xfffff000);
		for(uint tbl_i = 0; tbl_i < 1024; tbl_i++)
		{
			if(table[tbl_i] | 1)
				free_page(table[tbl_i] & 0xfffff000);
		}
		free_page((uint)table);
	}
	free_page((uint)task->page_directory);
	
	kfree(task);
}

void task_switch_isr(uint interrupt, uint error)
{
	interrupt = interrupt;
	error = error;
	task_switch();
}

uint task_find_next(uint t)
{
	for(uint i = 1; i <= MAX_TASKS; i++)
	{
		if(tasks[(i + t) % MAX_TASKS] != NULL && tasks[(i + t) % MAX_TASKS]->state == RUNNING)
			return (i + t) % MAX_TASKS;
	}
	panic("no tasks running");
	return 0;
}

void task_install_next_tss(uint t)
{	
	gdt_entry_t task_seg;
	memset(&task_seg, 0, sizeof(gdt_entry_t));
	gdt_raw_entry_t task_seg_raw;
	// setup TSS
	task_seg.base = (uint)&tasks[t]->tss;
	task_seg.limit = sizeof(tss_t);
	task_seg.executable = true;
	task_seg.accessed = true;
	task_seg.bits32 = true;
	task_seg.readwrite = false;
	task_seg.privilege = 3;
	gdt_encode(&task_seg, &task_seg_raw);
	((uchar*)&task_seg_raw)[5] = 0x89; // hack to set access byte.
	gdt_set_entry(0x90, &task_seg_raw);
}

/*
void task_switch()
{
	cli();
	for(uint i = 0; i < MAX_TASKS; i++)
	{
		uint next_task = (current_task + i) % MAX_TASKS;
		if(tasks[next_task] != NULL)
			current_task = next_task;
	}
	gdt_entry_t task_seg;
	memset(&task_seg, 0, sizeof(gdt_entry_t));
	gdt_raw_entry_t task_seg_raw;
	// setup TSS
	task_seg.base = (uint)&tasks[current_task]->tss;
	task_seg.limit = sizeof(tss_t);
	task_seg.executable = true;
	task_seg.accessed = true;
	task_seg.bits32 = true;
	task_seg.readwrite = false;
	task_seg.privilege = 3;
	gdt_encode(&task_seg, &task_seg_raw);
	((uchar*)&task_seg_raw)[5] = 0x89; // hack to set access byte.
	gdt_set_entry(0x90, &task_seg_raw);
	gdt_flush();
	
	idt_entry_t syscall;
	idt_entry_factory(&syscall, 0x30, 0, 0, GATE_TASK_32);
//	idt_set_gate(0x80, )
	
	kprintf("TSS ESP: %x, TSS SS: %x, TSS CS: %x, TSS EIP: %x\n", tasks[current_task]->tss.esp, tasks[current_task]->tss.ss, tasks[current_task]->tss.cs, tasks[current_task]->tss.eip);
	
	__asm__ volatile("push %0" :: "r"((uint)tasks[current_task]->tss.ss));
	__asm__ volatile("push %0" :: "r"(tasks[current_task]->tss.esp));
	__asm__ volatile("push %0" :: "r"(tasks[current_task]->tss.eflags | 0x200));
	__asm__ volatile("push %0" :: "r"((uint)tasks[current_task]->tss.cs));
	__asm__ volatile("push %0" :: "r"(tasks[current_task]->tss.eip));
	__asm__ volatile("	cli \n\
						mov eax, %0 \n\
						mov cr3, eax \n\
						mov ax, 0x90 \n\
						ltr ax \n\
						mov al, 0x20 \n\
						out 0x20, al \n\
						iret \n\
						" :: "r"(tasks[current_task]->page_directory));
}
*/

void task_peek(task_t* task, uint virtual, uint length, void* buffer)
{
	// @TODO - optimize
	for(uint i = 0; i < length; i++)
	{
		((char*)buffer)[i] = *(char*)paging_virtual_to_physical(task->page_directory, virtual + i);
	}
}
