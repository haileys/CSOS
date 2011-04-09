#include "trap.h"
#include "console.h"
#include "stddef.h"
#include "idt.h"

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

static void trap_handler(uint interrupt, uint error)
{
	uint cr2;
	__asm__("mov eax, cr2" : "=a"(cr2));
	kprintf("TRAP: %s, %d\n", traps[interrupt], interrupt == 14 ? cr2 : error);
	if(interrupt == TRAP_PAGE_FAULT)
	{
		panic("page fault.");
	}
}

void trap_init()
{
	for(uint i = 0; i < 31; i++)
		subscribe_isr(i, trap_handler);
}