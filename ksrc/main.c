#include "console.h"
#include "util.h"
#include "multiboot.h"
#include "string.h"
#include "io.h"
#include "panic.h"
#include "gdt.h"
#include "ata.h"
#include "kmalloc.h"
#include "stdbool.h"
#include "idt.h"
#include "kb.h"
#include "part.h"
#include "ata.h"
#include "fs/vfs.h"
#include "fs/fat.h"
#include "task.h"
#include "trap.h"
#include "paging.h"

gdtr_t* get_gdt();
void switch_to_user_mode();

struct cb
{
	char tmp[255];
	uint levels;
};

static void ring3_helloworld(uint interrupt, uint error)
{
	interrupt = interrupt;
	error = error;
	kprintf("Hello from ring3");
}

int kmain(struct multiboot_info* mbd, unsigned int magic)
{
	if(magic != 0x2BADB002)
	{
		panic("Multiboot did not pass correct magic number");
	}
	if(mbd->mem_upper < 16*1024*1024)
	{
		panic("Not enough memory");
	}
	char tmp[513];
	uchar boot_drive = (mbd->boot_device >> 24) & 0xff;
	
	paging_init();
	
	console_init();
	console_clear();
	
	kprintf("\n\nWelcome to CSOS. Booted by %s, running on %s\n\n", mbd->boot_loader_name, cpuid(tmp));
	
	kprintf("Mem lower: %d, mem upper: %d. Total memory: %d MiB\n\n", mbd->mem_lower, mbd->mem_upper, (mbd->mem_upper + mbd->mem_lower) / 1024);
	kmalloc_init(0x00200000, 0x00400000);
	
	gdt_init();
	cli();
	idt_init();
	trap_init();
	sti();
	kb_init();
	
	task_init(0x00800000, mbd->mem_upper);
	
	partition_t part;
	part_read(boot_drive, 0, &part);
	
	kprint("Initializing FAT filesystem... ");
	vfs_init(fat_vfs(boot_drive, &part));
	kprint("Ok.\n");
	
	//
	// let's get to ring3
	//
	
	gdt_entry_t r3_code;
	gdt_raw_entry_t r3_craw;
	gdt_code_entry_factory(&r3_code, 0, 0xffffffff, 3, false, true);
	gdt_encode(&r3_code, &r3_craw);
	gdt_set_entry(0x80, &r3_craw);
	
	gdt_entry_t r3_data;
	gdt_raw_entry_t r3_draw;
	gdt_data_entry_factory(&r3_data, 0, 0xffffffff, 3, true);
	gdt_encode(&r3_data, &r3_draw);
	gdt_set_entry(0x88, &r3_draw);	
	
	tss_t r3_tss;
	
	gdt_entry_t r3_tss_seg;
	gdt_raw_entry_t r3_tss_raw;
	r3_tss_seg.base = (uint)&r3_tss;
	r3_tss_seg.limit = sizeof(tss_t);
	r3_tss_seg.executable = true;
	r3_tss_seg.accessed = true;
	r3_tss_seg.bits32 = true;
	r3_tss_seg.readwrite = false;
	r3_tss_seg.privilege = 3;
	gdt_encode(&r3_tss_seg, &r3_tss_raw);
	((uchar*)&r3_tss_raw)[5] = 0x89; // hack to set access byte.
	gdt_set_entry(0x90, &r3_tss_raw);
	
	char* kernel_stack_pointer = kmalloc(0x10000);
	r3_tss.ss0 = 0x10;
	r3_tss.esp0 = (uint)kernel_stack_pointer + 0xfff0;
	r3_tss.iopb = sizeof(tss_t);
	__asm__("ltr ax" :: "a"(0x90));
	
	gdt_flush();
	
	kprintf("Set up relevant GDT entries for ring3.\nJumping to ring3...\n");
	
	subscribe_isr(0x80, ring3_helloworld);
	idt_set_privilege(0x80, 3);
	
	switch_to_user_mode();
	
	while(true)
	{
		__asm__("hlt");
	}
}