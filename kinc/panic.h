#ifndef PANIC_H
#define PANIC_H

#include "stddef.h"

void panic(char* message);
void panicf(char* fmt, ...);

#endif