#include "idt.h"
#include "console.h"
#include "string.h"
#include "io.h"
#include "console.h"
#include "kmalloc.h"

static idt_entry_t* IDT = (idt_entry_t*)0x00001600;
idtr_t idtr;

static bool int_state_stack[16];
static bool* int_state_stack_ptr = int_state_stack;

static void(*isr_handlers[256])(uint,uint);

void idt_entry_factory(idt_entry_t* entry, ushort selector, uint offset, uchar dpl, idt_gate_type_t gate_type)
{
	entry->offset_high = (offset >> 16) & 0xffff;
	entry->offset_low = offset & 0xffff;
	entry->selector = selector;
	entry->zero = 0;
	entry->type	= (1 << 7) // Present
				| ((dpl & 0x3) << 5) // DPL
				| (0 << 4) // storage segment (wtf is that)
				| (gate_type & 0xf);
}

//
/* assembly interface stuffola */
uint isr_dispatch(uint interrupt, uint errorcode)
{	
	if(interrupt == 39) // spurious IRQ 7
		return interrupt;
	
	if(isr_handlers[interrupt])
		isr_handlers[interrupt](interrupt, errorcode);
	
	// for debugging purposes
//	if(interrupt != 32)
//		kprintf("Interrupt %d, error %x\n", interrupt, errorcode);
	
	return interrupt; // important for the assembly handler
}
void idt_register_handler(uint interrupt, uint handler_ptr)
{
	idt_entry_factory(IDT + interrupt, 0x08, handler_ptr, 0, GATE_INT_32);
}
void idt_set_privilege(uint interrupt, uchar dpl)
{
	uchar type = IDT[interrupt].type;
	type &= ~(0x3 << 5);
	type |= (dpl & 0x3) << 5;
	IDT[interrupt].type = type;
}
/* - fin - */
//

void begin_no_ints()
{
	*int_state_stack_ptr++ = interrupts_enabled();
	cli();
}

void end_no_ints()
{
	if(*--int_state_stack_ptr)
		sti();
}

void subscribe_isr(uchar interrupt, void(*isr)(uint interrupt, uint errorcode))
{
	if(isr_handlers[interrupt])
		panicf("Attempted to subscribe to already subscribed interrupt: 0x%x", interrupt);
		
	isr_handlers[interrupt] = isr;
}
void unsubscribe_isr(uchar interrupt)
{
	if(!isr_handlers[interrupt])
		panicf("Attempted to unsubscribe null ISR: 0x%x", interrupt);
		
	isr_handlers[interrupt] = NULL;
}

void asm_isr_init();

void idt_init()
{
	cli();
	memset(IDT, 0, 256 * 8);
	
	// init the handler thingies in assembly
	asm_isr_init();
	
	idt_map_irqs();
	
	idtr.limit = 256*8 - 1;
	idtr.base = (uint)IDT;
	__asm__("lidt idtr");
	sti();
}

void idt_map_irqs()
{	
	// remap IRQ table
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}
