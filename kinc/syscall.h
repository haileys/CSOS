#ifndef SYSCALL_H
#define SYSCALL_H

#include "task.h"

void syscall_handler(regs_t* registers);

void syscall_isr();

#endif