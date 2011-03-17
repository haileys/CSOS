#ifndef REALMODE_H
#define REALMODE_H

#include "stddef.h"

typedef struct regs_16
{
	ushort ax;
	ushort bx;
	ushort cx;
	ushort dx;
	
	ushort si;
	ushort di;
}
__attribute__((__packed__)) regs_16_t;

// executes code in a real-mode context. Code runs with DS and CS == 0. Code must be ORGed to 0x9000
void real_exec(void* codebuffer, size_t len);
void bios_int(uchar interrupt, regs_16_t* regs);

#endif