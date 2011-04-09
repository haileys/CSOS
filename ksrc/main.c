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

void kmain(struct multiboot_info* mbd, unsigned int magic)
{
	if(magic != 0x2BADB002)
	{
		panic("Multiboot did not pass correct magic number");
	}
	if(mbd->mem_upper < 16*1024)
	{
		panic("Not enough memory");
	}
	char tmp[513];
	uchar boot_drive = (mbd->boot_device >> 24) & 0xff;
	
	paging_init((mbd->mem_upper + mbd->mem_lower + 1023) / 1024);
	
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
	
	uint init_size = vfs_size("/init.bin");
	char* buff = kmalloc(((init_size + 4095) / 4096) * 4096);
	vfs_readfile("/init.bin", 0, init_size, buff);
	task_create(init_size, buff, 4096);
	kfree(buff);
	
	cli();
	idt_register_handler(32, (uint)task_switch);
	sti();
	
	__asm__("hlt");
	panic("init exited");
}