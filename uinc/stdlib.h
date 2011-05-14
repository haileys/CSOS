#ifndef STDLIB_H
#define STDLIB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <csos.h>

// minimal stdlib implementation. will grow over time.

int rand();
void srand(uint seed);

int abs(int);
long int labs(long int);

typedef struct
{
	int quot, rem;
}
div_t;

typedef struct
{
	long int quot, rem;
} 
ldiv_t;

div_t div(int, int);
ldiv_t ldiv(long int, long int);

void exit(int status);

#endif