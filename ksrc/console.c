#include "console.h"
#include "string.h"
#include "io.h"
#include "stdarg.h"

static uint col = 0;
static uint row = 0;
static uint width = 80;
static uint height = 25;

static char* vram = (char*)0xb8000;

//extern char bin_console16[];
//extern size_t bin_console16_len;

static void console_cursor(uint col, uint row)
{
	ushort pos = (row * width) + col;
	ushort base_vga_port = *(ushort*)0x463; // read base vga port from bios data area
	
	outb(base_vga_port, 0x0E);
	outb(base_vga_port + 1, (pos >> 8) & 0xFF);
	
	outb(base_vga_port, 0x0F);
	outb(base_vga_port + 1, pos & 0xFF);
}

void console_init()
{
	console_cursor(col, row);
}

void console_gotoxy(uint x, uint y)
{
	col = x % width;
	row = y % height;
	console_cursor(col, row);
}

void console_movexy(int x, int y)
{
	col = (col + x) % width;
	row = (row + y) % height;
	console_cursor(col, row);
}

void kprintc(color_t bg, color_t fg, char* str)
{
	while(*str)
	{
		if(*str != '\n')
		{
			vram[(row * width + col) * 2] = *str;
			vram[(row * width + col) * 2 + 1] = (bg << 4) | (fg & 0xf);
			col++;
		}
		else
		{
			col = 0;
			row++;
		}
		if(col == width)
		{
			col = 0;
			row++;
		}
		if(row == height)
		{
			memmove(vram, vram + (width * 2), (height - 1) * width * 2);
			for(uint i = width*(height-1)*2; i < width*height*2; i += 2)
				vram[i] = 0;
			row = height - 1;
		}
		str++;
	}
	console_cursor(col, row);
}

void kprint(char* str)
{
	kprintc(BLACK, LTGRAY, str);
}
void kprintf(char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	char buff[400];
	vsnprintf(buff, 400, fmt, va);
	kprintc(BLACK, LTGRAY, buff);
}
void kprintfc(color_t bg, color_t fg, char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	char buff[400];
	vsnprintf(buff, 400, fmt, va);
	kprintc(bg, fg, buff);
}
void console_clear()
{
	for(size_t i = 0; i < width*height*2; i += 2)
	{
		vram[i] = 0;
		vram[i + 1] = 7;
	}
}