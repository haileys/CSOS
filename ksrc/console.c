#include "console.h"
#include "string.h"
#include "io.h"
#include "stdarg.h"
#include "kmalloc.h"

#define UNUSED __attribute__((unused)) 

static uint col = 0;
static uint row = 0;
static uint width = 80;
static uint height = 25;

static char* vram = (char*)0xb8000;

//extern char bin_console16[];
//extern size_t bin_console16_len;

vfs_stream_t console_stdout_stream;
vfs_stream_t* console_stdout()
{
	return &console_stdout_stream;
}

static uint console_vfs_read(UNUSED void* state, UNUSED uint len, UNUSED void* buffer)
{
	return 0;
}

static uint console_vfs_write(UNUSED void* state, uint len, void* buffer)
{
	for(uint i = 0; i < len; i++)
	{
		if(((char*)buffer)[i] == 0)
		{
			kprint(buffer);
			return len;
		}
	}
	char* new = (char*)kmalloc(len + 1);
	memcpy(new, buffer, len);
	new[len] = 0;
	kprint(buffer);
	return len;
}

static void console_vfs_seek(UNUSED void* state, UNUSED int offset, UNUSED vfs_seek_t seek) { }
static void console_vfs_close(UNUSED void* state) { }

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
	console_stdout_stream.read = console_vfs_read;
	console_stdout_stream.write = console_vfs_write;
	console_stdout_stream.seek = console_vfs_seek;
	console_stdout_stream.close = console_vfs_close;
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