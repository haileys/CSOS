#ifndef STRING_H
#define STRING_H

#include "stddef.h"
#include "stdarg.h"

void* memcpy(void* dest, void* src, size_t n);
void* memmove(void* dest, void* src, size_t n);
void* memset(void* mem, char c, size_t len);
int memcmp(void* a, void* b, size_t len);

size_t strlen(char* str);
char* strcpy(char* dest, char* src);
int strcmp(char* a, char* b);
char* strncpy(char* dest, char* src, size_t len);

char* strntrim(char* buff, size_t len, char* str);

char* vsnprintf(char* str, size_t len, char* fmt, va_list va);
char* snprintf(char* str, size_t len, char* fmt, ...);

char* strtoupper(char* dest, char* src);

#endif