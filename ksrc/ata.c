#include "ata.h"
#include "realmode.h"
#include "string.h"
#include "panic.h"

extern char bin_ata16[];
extern size_t bin_ata16_len;

struct lba_packet
{
	uchar size;
	uchar reserved;
	ushort sectors;
	ushort dest_offset;
	ushort dest_segment;
	uint lba_lo;
	uint lba_hi;
};

bool ata_read_sector(uchar disk, uint lba, void* buffer)
{
	struct lba_packet* packet = (struct lba_packet*)0x2000;
	packet->size = 16;
	packet->reserved = 0;
	packet->sectors = 1;
	packet->dest_segment = 0x0000;
	packet->dest_offset = 0x2200;
	packet->lba_lo = lba;
	packet->lba_hi = 0;
	
	*(uchar*)0x2010 = disk; // disk
	*(uchar*)0x2011 = 0x42; // read
	
	real_exec(bin_ata16, bin_ata16_len);
	
	memcpy(buffer, (void*)0x2200, 512);
	
	bool success = !(*(bool*)0x2000);
	if(!success)
		panicf("Read failed. LBA = %d, buffer = 0x%x", lba, buffer);
		
	return success;
}

bool ata_write_sector(uchar disk, uint lba, void* buffer)
{	
	memcpy((void*)0x2200, buffer, 512);
	
	struct lba_packet* packet = (struct lba_packet*)0x2000;
	packet->size = 16;
	packet->reserved = 0;
	packet->sectors = 1;
	packet->dest_segment = 0x0000;
	packet->dest_offset = 0x2200;
	packet->lba_lo = lba;
	packet->lba_hi = 0;
	
	*(uchar*)0x2010 = disk; // disk
	*(uchar*)0x2011 = 0x43; // read
	
	real_exec(bin_ata16, bin_ata16_len);
	
	bool success = !(*(bool*)0x2000);
	if(!success)
		panic("Disk write unsuccesful.");
		
	return success;
}

bool ata_read_sectors(uchar disk, uint lba, uint sectors, void* buffer)
{
	uint at_a_time = 64;
	if(sectors > at_a_time)
	{
		for(uint i = 0; i < sectors; i += at_a_time)
		{
			if(!ata_read_sectors(disk, lba + i, (sectors - i) > at_a_time ? at_a_time : sectors - i, (char*)buffer + (i * 512)))
				return false;
		}
		return true;
	}
	struct lba_packet* packet = (struct lba_packet*)0x2000;
	packet->size = 16;
	packet->reserved = 0;
	packet->sectors = sectors;
	packet->dest_segment = 0x1000;
	packet->dest_offset = 0x0000;
	packet->lba_lo = lba;
	packet->lba_hi = 0;
	
	*(uchar*)0x2010 = disk; // disk
	*(uchar*)0x2011 = 0x42; // read
	
	real_exec(bin_ata16, bin_ata16_len);
	
	memcpy(buffer, (void*)0x10000, 512 * sectors);
	
	bool success = !(*(bool*)0x2000);
	if(!success)
		panicf("Read failed. LBA = %d, buffer = 0x%x", lba, buffer);
		
	return success;
}

bool ata_write_sectors(uchar disk, uint lba, uint sectors, void* buffer)
{
	// optimize later
	for(uint i = 0; i < sectors; i++)
	{
		if(!ata_write_sector(disk, lba + i, (char*)buffer + 512*i))
			return false;
	}
	return true;
}
