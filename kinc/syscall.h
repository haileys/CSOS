#ifndef SYSCALL_H
#define SYSCALL_H

#include "task.h"

void sys_alloc_page(regs_t* registers);
void sys_free_page(regs_t* registers);
void sys_exit(regs_t* registers);
void sys_kill(regs_t* registers);
void sys_get_pid(regs_t* registers);
void sys_fork(regs_t* registers);
void sys_write(regs_t* registers);
void sys_read(regs_t* registers);
void sys_open(regs_t* registers);
void sys_close(regs_t* registers);
void sys_write_ebx(regs_t* registers);

void syscall_handler(regs_t* registers);

void syscall_isr();

#endif