#ifndef PAGING_H
#define PAGING_H

#include "stddef.h"
#include "stdbool.h"

extern uint page_directory[1024];

void paging_init(uint mb); // megabytes to identity map for kernel

void paging_map(uint virtual, uint physical, bool user);
void paging_unmap(uint virtual);

uint paging_virtual_to_physical(uint* page_directory, uint virtual);

#endif