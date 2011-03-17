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
typedef uint(*function_pointer_t)(uint,...);
function_pointer_t function_by_name(char* name);

void sleep(uint ms);

#endif