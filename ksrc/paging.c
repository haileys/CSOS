#include "paging.h"
#include "kmalloc.h"
#include "console.h"
#include "string.h"
#include "panic.h"
#include "stddef.h"
#include "stdbool.h"

uint page_directory[1024] __attribute__((aligned(0x1000)));

void paging_init(uint mb)
{
	memset(page_directory, 0, sizeof(uint) * 1024);
	for(uint dir_i = 0; dir_i < (mb + 3) / 4; dir_i++)
	{
		uint* table = (uint*)(0x00400000 + (4096 * dir_i));
		page_directory[dir_i] = ((uint)table & 0xfffff000) | 1;
		
		for(uint tbl_i = 0; tbl_i < 1024; tbl_i++)
		{
			table[tbl_i] = ((dir_i * 1024 + tbl_i) * 4096) | 1;
		}
	}
	
	__asm__ volatile("mov cr3, eax" :: "a"(page_directory));
	
	__asm__ volatile("mov eax, cr0\n"
					 "or eax, 0x80000000\n"
					 "mov cr0, eax" ::: "eax");
}

void paging_map(uint virtual, uint physical, bool user)
{
	if(virtual & 0xfff)
		panic("Virtual address not page-aligned");
	if(physical & 0xff)
		panic("Physical address not page-aligned");
	
	virtual /= 4096;
	uint dir_i = virtual / 1024;
	uint tbl_i = virtual % 1024;
	
	uint* table = (uint*)(page_directory[dir_i] & 0xfffff000);
	
	if(table[tbl_i] & 1)
		panic("Tried to map already mapped page table entry");
		
	table[tbl_i] = (physical & 0xfffff000) | 1 | 2 | (user ? 4 : 0);
	
	__asm__ volatile("mov cr3, eax" :: "a"(page_directory)); // invalidate TLB cache
}

void paging_unmap(uint virtual)
{
	if(virtual & 0xfff)
		panic("Virtual address not page-aligned");
		
	virtual /= 4096;
	uint dir_i = virtual / 1024;
	uint tbl_i = virtual % 1024;
		
	uint* table = (uint*)(page_directory[dir_i] & 0xfffff000);
	
	if(!(table[tbl_i] & 1))
		panic("Tried to unmap unmapped page table entry");
	
	table[tbl_i] = 0;
	
	__asm__ volatile("mov cr3, eax" :: "a"(page_directory)); // invalidate TLB cache
}

uint paging_virtual_to_physical(uint* page_directory, uint virtual)
{
	uint* table = (uint*)(page_directory[(virtual / 4096) / 1024] & 0xfffff000);
	uint phys = table[(virtual / 4096) % 1024] & 0xfffff000;
	return phys | (virtual & 0xfff);
}