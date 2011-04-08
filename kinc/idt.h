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

void idt_entry_factory(idt_entry_t* entry, ushort selector, uint offset, uchar dpl, idt_gate_type_t gate_type);
void idt_set_gate(uchar gate, idt_entry_t* ent);
void idt_set_privilege(uint interrupt, uchar dpl);

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

void subscribe_isr(uchar interrupt, void(*isr)(uint, uint));
void unsubscribe_isr(uchar interrupt);

void idt_init();
void idt_map_irqs();
void idt_register_handler(uint interrupt, uint handler_ptr);

#endif