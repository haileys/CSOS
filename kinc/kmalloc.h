#ifndef KMALLOC_H
#define KMALlOC_H

#include "stddef.h"

void mm_init();

#ifdef MALLOC_PRINT_DEBUG
	#define kmalloc(len) _kmalloc(len, __FILE__, __LINE__)
	#define kfree(len) _kfree(len, __FILE__, __LINE__)
	void* _kmalloc(size_t len, char* file, uint line);
	void _kfree(void* ptr, char* file, uint line);
#else
	#define kmalloc(len) _kmalloc(len)
	#define kfree(len) _kfree(len)
	void* _kmalloc(size_t len);
	void _kfree(void* ptr);
#endif

#endif