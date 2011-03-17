#include "gdt.h"
#include "stdbool.h"
#include "string.h"
#include "console.h"
#include "panic.h"

gdt_raw_entry_t* GDT = (gdt_raw_entry_t*)0x00000600;

void gdt_code_entry_factory(gdt_entry_t* entry, uint base, uint size, ushort privilege, bool conforming, bool readable)
{
	entry->base = base;
	if(size > 65536)
	{
		entry->granularity = GR_4KIB;
		entry->limit = size / 4096;
	}
	else
	{
		entry->granularity = GR_BYTE;
		entry->limit = size;
	}
	entry->readwrite = readable;
	entry->conforming = conforming;
	entry->executable = true;
	entry->privilege = privilege & 0x03;
	entry->bits32 = true;
}
void gdt_data_entry_factory(gdt_entry_t* entry, uint base, uint size, ushort privilege, bool writable)
{
	entry->base = base;
	if(size > 65536)
	{
		entry->granularity = GR_4KIB;
		entry->limit = size / 4096;
	}
	else
	{
		entry->granularity = GR_BYTE;
		entry->limit = size;
	}
	entry->readwrite = writable;
	entry->conforming = 0;
	entry->executable = false;
	entry->privilege = privilege & 0x03;
	entry->bits32 = true;
}

void gdt_init()
{
	gdt_entry_t entry;
	gdt_raw_entry_t raw;
	
	memset(&entry, 0, sizeof(gdt_entry_t));
	gdt_encode(&entry, &raw);
	gdt_set_entry(0, &raw);
	
	gdt_code_entry_factory(&entry, 0, 0xffffffff, 0, false, true);
	gdt_encode(&entry, &raw);
	gdt_set_entry(0x08, &raw);
	
	gdt_data_entry_factory(&entry, 0, 0xffffffff, 0, true);
	gdt_encode(&entry, &raw);
	gdt_set_entry(0x10, &raw);
	
	// setup selectors for real mode transition
	gdt_code_entry_factory(&entry, 0, 0xffff, 0, false, true);
	entry.bits32 = false;
	gdt_encode(&entry, &raw);
	gdt_set_entry(0x18, &raw);
	
	gdt_data_entry_factory(&entry, 0, 0xffff, 0, true);
	entry.bits32 = false;
	gdt_encode(&entry, &raw);
	gdt_set_entry(0x20, &raw);
	
	gdt_flush();
}
void gdt_flush()
{
	gdtr_t gdtr;
	gdtr.size = 0x1000;
	gdtr.offset = (uint)GDT;
	__asm__("lgdt %0" :: "m"(gdtr));
	gdt_reload_segment_registers();
}
gdt_raw_entry_t* gdt_get_entry(ushort selector)
{
	return GDT + selector/8;
}
void gdt_set_entry(ushort selector, gdt_raw_entry_t* raw)
{
	memcpy(&(GDT[selector / 8]), raw, 8);
}
ushort gdt_add_entry(gdt_raw_entry_t* raw)
{
	ushort i;
	for(i = 0; i < 512; i++)
	{
		char access_byte = *(char*)(GDT + i) + 5;
		if(!(access_byte >> 7)) // zero present bit means this selector is available for use
		{
			memcpy(&(GDT[i]), raw, 8);
			return i * 8;
		}
	}
	return GDT_NULL_SELECTOR;
}
void gdt_del_entry(ushort selector)
{
	*((char*)(GDT + selector / 8) + 5) &= ~0x80; // disable present bit
}

void gdt_encode(gdt_entry_t* entry, gdt_raw_entry_t* raw)
{
	if(entry->limit >> 20)
		panic("GDT entry limit must fit in 20 bits");
	if(entry->granularity != GR_BYTE && entry->granularity != GR_4KIB)
		panic("Unknown granularity in GDT entry");
	
	uchar* record = (uchar*)raw;
	// limit
	record[0] = entry->limit & 0xff;
	record[1] = (entry->limit >> 8) & 0xff;
	// base
	record[2] = entry->base & 0xff;
	record[3] = (entry->base >> 8) & 0xff;
	record[4] = (entry->base >> 16) & 0xff;
	record[7] = (entry->base >> 24) & 0xff;
	// encode granularity and bits 16 - 19 of limit
	uchar flags = entry->limit >> 16;
	if(entry->bits32)
		flags |= 0x40; // set size to 32 bit
	if(entry->granularity == GR_4KIB)
		flags |= 0x80;
	record[6] = flags;
	// here's the fun bit: encoding the type byte!
	uchar type = 0;
	if(entry->readwrite)
		type |= 0x02;
	if(entry->conforming)
		type |= 0x04;
	if(entry->executable)
		type |= 0x08;
	// this bit is reserved. it must be 1
	type |= 0x10;
	// privilege is 2 bits. mask it out, shift it up and or it in
	type |= (entry->privilege & 0x03) << 5;
	// set present bit to 1
	type |= 0x80;
	record[5] = type;
}
void gdt_decode(gdt_raw_entry_t* raw, gdt_entry_t* entry)
{	
	uchar* record = (uchar*)raw;
	
	entry->limit = ((record[6] & 0x0f) << 16) | (record[1] << 8) | record[0];
	entry->base = (record[7] << 24) | (record[4] << 16) | (record[3] << 8) | record[2];
	
	entry->bits32 = (record[6] & 0x40) ? true : false;
	entry->granularity = (record[6] & 0x80) ? GR_4KIB : GR_BYTE;
		
	entry->accessed = (record[5] & 0x01) ? true : false;
	entry->readwrite = (record[5] & 0x02) ? true : false;
	entry->conforming = (record[5] & 0x04) ? true : false;
	entry->executable = (record[5] & 0x08) ? true : false;
	entry->privilege = (record[5] >> 5) & 0x03;
}