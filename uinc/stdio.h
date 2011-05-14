#ifndef STDIO_H
#define STDIO_H

#include <stdlib.h>
#include <stdarg.h>

typedef struct FILE
{
	uint fd;
}
FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

size_t fwrite(void* buff, size_t size, size_t count, FILE* stream);
size_t fread(void* buff, size_t size, size_t count, FILE* stream);

int fprintf(FILE* stream, char* fmt, ...);
int vfprintf(FILE* stream, char* fmt, va_list vargs);
int printf(char* fmt, ...);
int vprintf(char* fmt, va_list vargs);
int sprintf(char* buff, char* fmt, ...);
int snprintf(char* buff, size_t len, char* fmt, ...);
int vsprintf(char* buff, char* fmt, va_list vargs);
int vsnprintf(char* str, size_t len, char* fmt, va_list va);

#endif