#include "kb.h"
#include "stddef.h"
#include "idt.h"
#include "io.h"
#include "stdbool.h"
#include "console.h"
#include "kmalloc.h"

static bool kb_isr_escaped = false;

static bool escape = false;
static bool key_ready = false;
static uchar key;

static bool shift = false;
static bool ctrl = false;
static bool alt = false;

static uchar uppercase_keys[256] = { 0 };
static uchar lowercase_keys[256] = { 0 };

static void kb_isr(uint interrupt, uint error, void* state)
{
	// make gcc shutup
	interrupt = interrupt;
	error = error;
	state = state;
	
	uchar k = inb(0x60);
	if(kb_isr_escaped)
	{
		kb_isr_escaped = false;
		escape = true;
		key_ready = true;
		key = k;
	}
	else
	{
		if(k == 0xe0)
		{
			kb_isr_escaped = true;
			return;
		}
		escape = false;
		key = k;
		key_ready = true;
	}
	switch(k)
	{
		case 0x1d:
			ctrl = true;
			break;
		case 0x9d:
			ctrl = false;
			break;
		case 0x38:
			alt = true;
			break;
		case 0xb8:
			alt = false;
			break;
		case 0x2a:
			shift = true;
			break;
		case 0xaa:
			shift = false;
			break;
	}
}

void kb_us_keymap();
void kb_init()
{
	subscribe_isr(33, NULL, kb_isr);
	kb_us_keymap();
}

scancode_t kb_read_scancode()
{
	key_ready = false;
	while(!key_ready)
		__asm__("hlt");
	scancode_t sc;
	sc.escaped = escape;
	sc.key = key;
	return sc;
}

key_t kb_read_key()
{
	scancode_t sc = kb_read_scancode();
	key_t key;
	key.ctrl = ctrl;
	key.shift = shift;
	key.alt = alt;
	
	if(sc.escaped)
	{
		key.special = true;
		switch(sc.key)
		{
			case 0x48:
				key.key.special = AK_UP;
				return key;
			case 0x50:
				key.key.special = AK_DOWN;
				return key;
			case 0x4b:
				key.key.special = AK_LEFT;
				return key;
			case 0x4d:
				key.key.special = AK_RIGHT;
				return key;
			default:
				return kb_read_key(); // unknown scancode so we'll try again
		}
	}
	else
	{	
		key.special = false;
		switch(sc.key)
		{
			case 0x0e:
				key.special = true;
				key.key.special = BACKSPACE;
				break;
			default:
				key.key.ascii = (shift ? uppercase_keys : lowercase_keys)[sc.key];
				break;				
		}
		return key;
	}
}

// copy paste phillip larksons keymap
void kb_us_keymap()
{
        uppercase_keys[0x02] = '!';
        uppercase_keys[0x03] = '@';
        uppercase_keys[0x04] = '#';
        uppercase_keys[0x05] = '$';
        uppercase_keys[0x06] = '%';
        uppercase_keys[0x07] = '^';
        uppercase_keys[0x08] = '&';
        uppercase_keys[0x09] = '*';
        uppercase_keys[0x0a] = '(';
        uppercase_keys[0x0b] = ')';
        uppercase_keys[0x0c] = '_';
        uppercase_keys[0x0d] = '+';
        uppercase_keys[0x0e] = 0x08;
 
        uppercase_keys[0x10] = 'Q';
        uppercase_keys[0x11] = 'W';
        uppercase_keys[0x12] = 'E';
        uppercase_keys[0x13] = 'R';
        uppercase_keys[0x14] = 'T';
        uppercase_keys[0x15] = 'Y';
        uppercase_keys[0x16] = 'U';
        uppercase_keys[0x17] = 'I';
        uppercase_keys[0x18] = 'O';
        uppercase_keys[0x19] = 'P';
 
        uppercase_keys[0x1a] = '{';
        uppercase_keys[0x1b] = '}';
        uppercase_keys[0x1c] = '\n';
 
        uppercase_keys[0x1e] = 'A';
        uppercase_keys[0x1f] = 'S';
        uppercase_keys[0x20] = 'D';
        uppercase_keys[0x21] = 'F';
        uppercase_keys[0x22] = 'G';
        uppercase_keys[0x23] = 'H';
        uppercase_keys[0x24] = 'J';
        uppercase_keys[0x25] = 'K';
 
        uppercase_keys[0x27] = ':';
        uppercase_keys[0x28] = '"';
 
        uppercase_keys[0x26] = 'L';
        uppercase_keys[0x2c] = 'Z';
        uppercase_keys[0x2d] = 'X';
        uppercase_keys[0x2e] = 'C';
        uppercase_keys[0x2f] = 'V';
        uppercase_keys[0x30] = 'B';
        uppercase_keys[0x31] = 'N';
        uppercase_keys[0x32] = 'M';
 
        uppercase_keys[0x33] = '<';
        uppercase_keys[0x34] = '>';
        uppercase_keys[0x35] = '?';
       
        // LOWERCASE LOWERCASE LOWERCASE
 
        lowercase_keys[0x02] = '1';
        lowercase_keys[0x03] = '2';
        lowercase_keys[0x04] = '3';
        lowercase_keys[0x05] = '4';
        lowercase_keys[0x06] = '5';
        lowercase_keys[0x07] = '6';
        lowercase_keys[0x08] = '7';
        lowercase_keys[0x09] = '8';
        lowercase_keys[0x0a] = '9';
        lowercase_keys[0x0b] = '0';
        lowercase_keys[0x0c] = '-';
        lowercase_keys[0x0d] = '=';
        lowercase_keys[0x0e] = 0x08;
 
        lowercase_keys[0x10] = 'q';
        lowercase_keys[0x11] = 'w';
        lowercase_keys[0x12] = 'e';
        lowercase_keys[0x13] = 'r';
        lowercase_keys[0x14] = 't';
        lowercase_keys[0x15] = 'y';
        lowercase_keys[0x16] = 'u';
        lowercase_keys[0x17] = 'i';
        lowercase_keys[0x18] = 'o';
        lowercase_keys[0x19] = 'p';
 
        lowercase_keys[0x1a] = '[';
        lowercase_keys[0x1b] = ']';
        lowercase_keys[0x1c] = '\n';
 
        lowercase_keys[0x1e] = 'a';
        lowercase_keys[0x1f] = 's';
        lowercase_keys[0x20] = 'd';
        lowercase_keys[0x21] = 'f';
        lowercase_keys[0x22] = 'g';
        lowercase_keys[0x23] = 'h';
        lowercase_keys[0x24] = 'j';
        lowercase_keys[0x25] = 'k';
        lowercase_keys[0x26] = 'l';
 
        lowercase_keys[0x27] = ';';
        lowercase_keys[0x28] = '\'';
 
        lowercase_keys[0x2c] = 'z';
        lowercase_keys[0x2d] = 'x';
        lowercase_keys[0x2e] = 'c';
        lowercase_keys[0x2f] = 'v';
        lowercase_keys[0x30] = 'b';
        lowercase_keys[0x31] = 'n';
        lowercase_keys[0x32] = 'm';
 
        lowercase_keys[0x33] = ',';
        lowercase_keys[0x34] = '.';
        lowercase_keys[0x35] = '/';
}