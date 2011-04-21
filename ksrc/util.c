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

void sleep(uint ms)
{
	// @TODO
	// sleep for how many ms
	for(uint i = 0; i < ms*5000; i++) ;
}
