#include "panic.h"
#include "stdarg.h"
#include "console.h"
#include "stddef.h"
#include "string.h"

char* get_symtable_addr();
char* symbols = NULL;

void panicf(char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	char buff[500];
	vsnprintf(buff, 500, fmt, va);
	panic(buff);
}

void panic_backtrace_step(size_t address)
{
	if(!symbols)
		symbols = get_symtable_addr() + 16;
	uint best_offset = 0;
	char* best = "?";
	for(int i = 0; i < 2048; i++)
	{
		uint offset = *(uint*)(symbols + (i * 32));
		char* symbol = (char*)(symbols + (i * 32) + 4);
		if(offset == 0)
			continue;
		if(offset > address)
			continue;
		if(offset < address && offset > best_offset)
		{
			best_offset = offset;
			best = symbol;
		}
	}
	
	kprintf("  0x%x: %s +%x\n", best_offset, best, address - best_offset);
}
