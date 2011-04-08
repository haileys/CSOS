#include "syscall.h"
#include "console.h"
#include "panic.h"

void syscall_handler(regs_t* registers)
{
	kprintf("[syscall] EAX = %x\n", registers->eax);
}