#ifndef GDT_H
#define GDT_H

#include "stddef.h"
#include "stdbool.h"

#define GDT_NULL_SELECTOR 65535

typedef struct gdtr
{
	ushort size;
	uint offset;
}
__attribute__((__packed__)) gdtr_t;

typedef enum gdt_granularity
{
	GR_BYTE,
	GR_4KIB,
}
gdt_granularity_t;

typedef struct gdt_entry
{
	uint limit; // limited to 20 bits
	uint base;
	
	gdt_granularity_t granularity;
	
	bool accessed;
	bool readwrite;
	bool conforming;
	bool executable;
	bool bits32;
	uchar privilege;
}
gdt_entry_t;

typedef long long gdt_raw_entry_t;

void gdt_reload_segment_registers();

void gdt_init();
void gdt_flush();
gdt_raw_entry_t* gdt_get_entry(ushort selector);
void gdt_set_entry(ushort selector, gdt_raw_entry_t* raw);
ushort gdt_add_entry(gdt_raw_entry_t* raw);
void gdt_del_entry(ushort selector);

void gdt_code_entry_factory(gdt_entry_t* entry, uint base, uint size, ushort privilege, bool conforming, bool readable);
void gdt_data_entry_factory(gdt_entry_t* entry, uint base, uint size, ushort privilege, bool writable);

void gdt_encode(gdt_entry_t* entry, gdt_raw_entry_t* raw);
void gdt_decode(gdt_raw_entry_t* raw, gdt_entry_t* entry);

#endif