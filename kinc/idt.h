#ifndef IDT_H
#define IDT_H

#include "stddef.h"
#include "stdbool.h"

typedef struct idtr
{
	ushort limit;
	uint base;
}
__attribute__((__packed__)) idtr_t;

typedef struct idt_entry
{
	ushort offset_low;
	ushort selector;
	uchar zero;
	uchar type;
	ushort offset_high;
}
__attribute__((__packed__)) idt_entry_t;

typedef enum idt_gate_type
{
	GATE_TASK_32 = 0x5,
	GATE_INT_16 = 0x6,
	GATE_TRAP_16 = 0x7,
	GATE_INT_32 = 0xe,
	GATE_TRAP_32 = 0xf,
}
idt_gate_type_t;

typedef struct isr_handler
{
	/* if this handler is the first in the handler chain,
	   then prev should contain a number less than 256
	   corresponding to the interrupt vector */
	struct isr_handler* prev;
	struct isr_handler* next;
	void* state;
	void(*handler)(uint interrupt, uint errorcode, void* state);
}
isr_handler_t;

void idt_entry_factory(idt_entry_t* entry, ushort selector, uint offset, uchar dpl, idt_gate_type_t gate_type);

inline static void cli()
{
	__asm__("cli");
}
inline static void sti()
{
	__asm__("sti");
}
inline static bool interrupts_enabled()
{
	uint f;
	__asm__ volatile("pushf \n pop %0" : "=g"(f));
	return f & (1 << 9);
}
void begin_no_ints();
void end_no_ints();

isr_handler_t* subscribe_isr(uchar interrupt, void* state, void(*isr)(uint interrupt, uint errorcode, void* state));
void unsubscribe_isr(isr_handler_t* handler);

void idt_init();

#endif