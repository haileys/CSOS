#include "part.h"
#include "string.h"
#include "ata.h"

bool part_read(uchar disk, uint partition, partition_t* part)
{
	uchar buff[512];
	ata_read_sector(disk, 0, buff);
	memcpy(part, buff + 0x1be + (partition * 0x10), sizeof(partition_t));
	return part->system_id != 0;
}

bool part_read_sector(uchar disk, partition_t* part, uint sector, void* buffer)
{
	return ata_read_sector(disk, part->start_lba + sector, buffer);
}

bool part_write_sector(uchar disk, partition_t* part, uint sector, void* buffer)
{
	return ata_write_sector(disk, part->start_lba + sector, buffer);
}

bool part_read_sectors(uchar disk, partition_t* part, uint sector, uint sectors, void* buffer)
{
	return ata_read_sectors(disk, part->start_lba + sector, sectors, buffer);
}

bool part_write_sectors(uchar disk, partition_t* part, uint sector, uint sectors, void* buffer)
{
	return ata_write_sectors(disk, part->start_lba + sector, sectors, buffer);
}

uint part_num_partitions(uchar disk)
{
	uchar buff[512];
	ata_read_sector(disk, 0, buff);
	uint total = 0;
	partition_t* part = (partition_t*)(buff + 0x1be);
	for(uint i = 0; i < 4; i++)
	{
		if(part[i].system_id)
			total++;
	}
	return total;
}

char* part_filesystem_name(uchar system_id)
{
	switch(system_id)
	{
		case 0x00:
		return "Unallocated Space";
		case 0x01:
		return "FAT12";
		case 0x07:
		return "NTFS";
		case 0x0b:
		return "FAT32";
		case 0x0c:
		return "FAT32 (LBA-mapped)";
		case 0x0e:
		return "FAT16 (LBA-mapped)";
		default:
		return "Unknown";
	}
}