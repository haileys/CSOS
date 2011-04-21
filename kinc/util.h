#ifndef UTIL_H
#define UTIL_H

#include "stddef.h"

typedef struct symbol
{
	uint addr;
	char name[28];
}
__attribute__((__packed__)) symbol_t;

char* cpuid(char* buffer);

void sleep(uint ms);

#endif
