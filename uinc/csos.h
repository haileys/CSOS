#ifndef CSOS_H
#define CSOS_H

typedef unsigned int uint;

void* sys_alloc_page();
void sys_free_page(void* page);

void sys_exit(int status);

void sys_kill(uint pid);
uint sys_get_pid();
uint sys_fork();

uint sys_write(uint fd, void* buff, uint len);
uint sys_read(uint fd, void* buff, uint len);
uint sys_open(char* path);
void sys_close(uint fd);

#endif
