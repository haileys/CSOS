#ifndef CONSOLE_H
#define CONSOLE_H

#include "stddef.h"

typedef enum color
{
	BLACK = 0,
	BLUE = 0,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LTGRAY = 7,
	DKGRAY = 8,
	LTBLUE = 9,
	LTGREEN = 10,
	LTCYAN = 11,
	LTRED = 12,
	LTMAGENTA = 13,
	YELLOW = 14,
	WHITE = 15,
}
color_t;

void kprint(char* str);
void kprintc(color_t bg, color_t fg, char* str);
void kprintf(char* fmt, ...);
void kprintfc(color_t bg, color_t fg, char* fmt, ...);
void console_gotoxy(uint x, uint y);
void console_movexy(int x, int y);
void console_clear();
void console_init();

#endif