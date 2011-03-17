#include "stddef.h"
#include "string.h"
#include "realmode.h"
#include "console.h"

extern char bin_code16[];
extern uint bin_code16_len;

extern char bin_int16[];
extern uint bin_int16_len;

void real_exec(void* codebuffer, size_t len)
{
	memcpy((void*)0x8000, bin_code16, bin_code16_len);
	memcpy((void*)0x9000, codebuffer, len);
	
	((void(*)())0x8000)();
}

void bios_int(uchar interrupt, regs_16_t* regs)
{
	memcpy((void*)0x2000, regs, sizeof(regs_16_t));
	*(uchar*)0x200C = interrupt;
	real_exec(bin_int16, bin_int16_len);
	memcpy(regs, (void*)0x2000, sizeof(regs_16_t));
}
