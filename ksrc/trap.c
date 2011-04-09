#include "trap.h"
#include "console.h"
#include "stddef.h"
#include "idt.h"
#include "panic.h"
#include "task.h"

static char* traps[] = {
	"Divide by zero", // 0
	"Debug", // 1
	"Non-maskable interrupt", // 2
	"Breakpoint", // 3
	"Overflow", // 4
	"Bound Range Exceeded", // 5
	"Invalid Opcode", // 6
	"Device Not Available", // 7
	"Double Fault", // 8
	"Coprocessor Segment Overrun", // 9
	"Invalid TSS", // 10
	"Segment Not Present", // 11
	"Stack-Segment Fault", // 12
	"General Protection Fault", // 13
	"Page Fault", // 14
	NULL,
	"Floating Point Exception (x87)", // 16
	"Alignment Check", // 17
	"Machine Check", // 18
	"Floating Point Exception (SIMD)", // 19
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"Security Exception"
};

static void trap_handler(uint interrupt, uint error, interrupt_stack_t* int_stack)
{
	uint cr2;
	__asm__("mov eax, cr2" : "=a"(cr2));
	if(interrupt == 13)
	{
		kprintf("[#%d] %s. Killed.\n", task_current()->pid, traps[interrupt]);
		task_kill_and_free(task_current());
		current_task = 0xffffffff;
		sti();
		multitasking_enabled = true;
		for(;;) ;
	}
	if(multitasking_enabled)
	{
		uint retn_addr = *(uint*)(task_current()->tss.esp0 - 5*4);
		*(uint*)(task_current()->tss.esp0 - 5*4) = task_current()->exception_handler;
	}
}

void trap_init()
{
	for(uint i = 0; i < 31; i++)
		subscribe_isr(i, trap_handler);
}
