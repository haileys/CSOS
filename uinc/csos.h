#ifndef CSOS_H
#define CSOS_H

#include <_PDCLIB_glue.h>

typedef unsigned int uint;

//extern uint stdin;
//extern uint stdout;
//extern uint stderr;

void* alloc_page();
void* alloc_pages(uint n);
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
