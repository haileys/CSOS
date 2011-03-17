#ifndef IO_H
#define IO_H

#include "stddef.h"
#include "panic.h"

static inline void outb(ushort port, uchar value)
{
	__asm__("out %1, %0" : : "a"(value), "Nd"(port) );
}

static inline uchar inb(ushort port)
{
	uchar ret;
	__asm__ volatile("in %0, %1" : "=a"(ret) : "Nd"(port) );
	return ret;
}

static inline uint inl(ushort port)
{
	uint ret;
	__asm__ volatile("in %0, %1" : "=a"(ret) : "Nd"(port) );
	return ret;
}

static inline void insl(ushort port, void* buffer, uint quads)
{
	uint* buff = (uint*)buffer;
	for(uint i = 0; i < quads; i++)
		*buff++ = inl(port);
}

#endif