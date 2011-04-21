#include "trap.h"
#include "console.h"
#include "stddef.h"
#include "idt.h"
#include "panic.h"
#include "task.h"

static char* traps[] = {
	"Divide by zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	NULL,
	"Floating Point Exception (x87)",
	"Alignment Check",
	"Machine Check",
	"Floating Point Exception (SIMD)",
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

extern uint multitasking_enabled;
static void trap_handler(uint interrupt, uint error)
{
	uint cr2;
	__asm__("mov eax, cr2" : "=a"(cr2));
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
