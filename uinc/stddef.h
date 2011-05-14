#ifndef STDDEF_H
#define STDDEF_H

#define NULL ((void*)0)

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef unsigned long size_t;
typedef unsigned long ptrdiff_t;
typedef unsigned short wchar_t;

#define offsetof(st, m) \
     ((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))

#endif