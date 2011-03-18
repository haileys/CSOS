#include "paging.h"
#include "kmalloc.h"
#include "console.h"
#include "string.h"

uint page_directory[1024] __attribute__((aligned(0x1000)));

void paging_init()
{
	// identity map first 8 MiB of ram as kernel space
	for(uint dir_i = 0; dir_i < 2; dir_i++)
	{
		uint* table = (uint*)(0x00400000 + (4096 * dir_i));
		page_directory[dir_i] = ((uint)table & 0xfffff000) | 1 | 4;
		
		for(uint tbl_i = 0; tbl_i < 1024; tbl_i++)
		{
			table[tbl_i] = ((dir_i * 1024 + tbl_i) * 4096) | 1 | 4;
		}
	}
	
	__asm__ volatile("mov cr3, eax" :: "a"(page_directory));
	
	__asm__ volatile("mov eax, cr0\n"
					 "or eax, 0x80000000\n"
					 "mov cr0, eax" ::: "eax");
}