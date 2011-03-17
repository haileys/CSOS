#include "util.h"
#include "stddef.h"
#include "string.h"

char* cpuid(char* buffer)
{
	uint ebx, edx, ecx;
	__asm__ volatile("cpuid" : "=b"(ebx), "=d"(edx), "=c"(ecx) : "a"(0));
	((uint*)buffer)[0] = ebx;
	((uint*)buffer)[1] = edx;
	((uint*)buffer)[2] = ecx;
	buffer[12] = 0;
	return buffer;
}

char* get_symtable_addr();

function_pointer_t function_by_name(char* name)
{
	symbol_t* symbols = (symbol_t*)(get_symtable_addr() + 16);
	
	for(int i = 0; i < 2048; i++)
	{
		if(!symbols[i].addr)
			return NULL;
		if(strcmp(symbols[i].name, name) == 0)
			return (function_pointer_t)symbols[i].addr;
	}
	return NULL;
}

void sleep(uint ms)
{
	// @TODO
	// sleep for how many ms
	for(uint i = 0; i < ms*5000; i++) ;
}