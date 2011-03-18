#include "task.h"

uint user_base;
uint total_pages;

void task_init(uint base_physical, uint high_memory)
{
	user_base = base_physical;
	total_pages = (base_physical - high_memory * 1024) / 4096;
}