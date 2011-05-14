#include <string.h>

uint strlen(char* str)
{
	uint len = 0;
	while(*str++) len++;
	return len;
}
void* memcpy(void* dest, void* src, size_t n)
{
	__asm__(
		"mov esi, eax\n"
		"mov edi, edx\n"
		"rep movsb"
		:: "ecx"(n), "eax"(src), "edx"(dest));
	return dest;
}