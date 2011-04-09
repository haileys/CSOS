#ifndef CSOS_H
#define CSOS_H

#define stdin 0
#define stdout 1

typedef unsigned int uint;

void* alloc_page();
void free_page(void* page);

void exit(int status);

void kill(uint pid);
uint get_pid();
uint fork();

uint write(uint fd, void* buff, uint len);
uint read(uint fd, void* buff, uint len);
uint open(char* path);
void close(uint fd);

#endif