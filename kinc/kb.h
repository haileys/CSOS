#ifndef KB_H
#define KB_H

#include "stddef.h"
#include "stdbool.h"

typedef struct scancode
{
	bool escaped;
	uchar key;
}
__attribute__((__packed__)) scancode_t;

typedef enum special_key
{
	AK_UP,
	AK_DOWN,
	AK_LEFT,
	AK_RIGHT,
	BACKSPACE,
}
special_key_t;

typedef struct key
{
	bool special;
	bool shift;
	bool ctrl;
	bool alt;
	union
	{
		uchar ascii;
		special_key_t special;
	}
	key;
}
__attribute__((__packed__)) key_t;


void kb_init();
scancode_t kb_read_scancode();
key_t kb_read_key();

#endif