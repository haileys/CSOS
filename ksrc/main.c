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

gdtr_t* get_gdt();

struct cb
{
	char tmp[255];
	uint levels;
};

static bool dir_callback(void* state, char* name, vfs_type_t type)
{
	if(strcmp(".", name) == 0 || strcmp("..", name) == 0)
		return true;
	for(uint i = 0; i < ((struct cb*)state)->levels; i++)
		kprintf("- ");
	kprintf("* %s\n", name);
	if(type == VFS_DIR)
	{
		struct cb st;
		st.levels = ((struct cb*)state)->levels + 1;
		strcpy(st.tmp, ((struct cb*)state)->tmp);
		st.tmp[strlen(((struct cb*)state)->tmp)] = '/';
		strcpy(st.tmp + 1 + strlen(((struct cb*)state)->tmp), name);
		st.tmp[(uint)st.tmp + 1 + strlen(((struct cb*)state)->tmp)] = 0;
		vfs_readdir(st.tmp, &st, dir_callback);
	}
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
	part_read(boot_drive, 0, &part);
		
	kprint("Initializing FAT filesystem... ");
	vfs_init(fat_vfs(boot_drive, &part));
	kprint("Ok.\n");
	
	char* filename = "/a.txt";
	kprintf("%s is %d bytes long. Contents:\n\n", filename, vfs_size(filename));
	uint read = vfs_readfile(filename, 0, 512, tmp);
	tmp[read] = 0;
	kprintf("%s", tmp);
	
	while(true)
	{
		__asm__("hlt");
	}
}