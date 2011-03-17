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

static isr_handler_t* isr_handlers[256];

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
	
	isr_handler_t* node = isr_handlers[interrupt];
	while(node)
	{
		node->handler(interrupt, errorcode, node->state);
		node = node->next;
	}
	
	// for debugging purposes
//	if(interrupt != 32)
//		kprintf("Interrupt %d, error %x\n", interrupt, errorcode);
	
	return interrupt; // important for the assembly handler
}
void idt_register_handler(uint interrupt, uint handler_ptr)
{
	idt_entry_factory(IDT + interrupt, 0x08, handler_ptr, 0, GATE_INT_32);
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

isr_handler_t* subscribe_isr(uchar interrupt, void* state, void(*isr)(uint interrupt, uint errorcode, void* state))
{
	isr_handler_t* h = (isr_handler_t*)kmalloc(sizeof(isr_handler_t));
	h->next = NULL;
	if(!isr_handlers[interrupt])
	{
		h->prev = (isr_handler_t*)(uint)interrupt; // this is somewhat of a hack
		isr_handlers[interrupt] = h;
	}
	else
	{
		isr_handler_t* node = isr_handlers[interrupt];
		while(node->next)
			node = node->next;
		h->prev = node;
		node->next = h;
	}
	h->state = state;
	h->handler = isr;
	return h;
}
void unsubscribe_isr(isr_handler_t* handler)
{
	if((uint)handler->prev >= 256)
	{
		handler->prev->next = handler->next;
	}
	else
	{
		isr_handlers[(uint)handler->prev] = handler->next;
	}
	
	kfree(handler);
}

void asm_isr_init();

void idt_init()
{
	memset(IDT, 0, 256 * 8);
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
	
	// init the handler thingies in assembly
	asm_isr_init();
	
	idtr.limit = 256*8 - 1;
	idtr.base = (uint)IDT;
	__asm__("lidt idtr");
}