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
<<<<<<< HEAD
#include "fat.h"
=======
#include "ata.h"
#include "fs/vfs.h"
#include "fs/fat.h"
>>>>>>> new

gdtr_t* get_gdt();

static bool dir_callback(void* state, char* name, vfs_type_t type)
{
	state = state;
	kprintf("  %s: %s\n", type == VFS_FILE ? "File" : "Dir", name);
	return true;
}

int kmain(struct multiboot_info* mbd, unsigned int magic)
{
	if(magic != 0x2BADB002)
	{
		panic("Multiboot did not pass correct magic number");
	}
	
	char tmp[512];
	uchar boot_drive = (mbd->boot_device >> 24) & 0xff;
	
	console_init();
	kb_init();
	console_clear();
	
	kprintf("Welcome to CSOS. Booted by %s, running on %s\n\n", mbd->boot_loader_name, cpuid(tmp));
	
	kprintf("Mem lower: %d, mem upper: %d. Total memory: %d MiB\n\n", mbd->mem_lower, mbd->mem_upper, (mbd->mem_upper - mbd->mem_lower) / 1024);
	mm_init(0x00200000, 0x00400000);
	
	gdt_init();
	cli();
	idt_init();
	sti();
	
	partition_t part;
<<<<<<< HEAD
	part_read(0x80, 0, &part);
	kprintf("Partition 0 boot flag: 0x%x\nPartition 0 system id: 0x%x - %s\nPartition 0 start LBA: %d\nPartition 0 size: %d MB\n",
		part.boot_flag, part.system_id, part_filesystem_name(part.system_id), part.start_lba, part.length / 2 / 1024);
		
	fat_bpb_t bpb;
	ata_read_sector(0x80, part.start_lba, &bpb);
	kprintf("OEM: %s\nFAT offset: %d sectors\nDir entry count: %d\n", bpb.oem, bpb.reserved_sectors, bpb.directory_entry_count);
=======
	part_read(boot_drive, 0, &part);
		
	kprint("Initializing FAT filesystem... ");
	vfs_init(fat_vfs(boot_drive, &part));
	kprint("Ok.\n");
	
	kprint("\nDir listing of /:\n");
	vfs_readdir("/", NULL, dir_callback);
	kprint("Done.\n");
>>>>>>> new
	
	while(true)
	{
		__asm__("hlt");
	}
}