#ifndef PART_H
#define PART_H

#include "stddef.h"
#include "stdbool.h"

typedef struct partition
{
	uchar boot_flag;
	uchar start_head;
	ushort start_sector_cylinder;
	uchar system_id;
	uchar end_head;
	ushort end_sector_cylinder;
	uint start_lba;
	uint length; // in sectors
}
__attribute__((__packed__)) partition_t;

bool part_read(uchar disk, uint partition, partition_t* part);
uint part_num_partitions(uchar disk);
char* part_filesystem_name(uchar system_id);

bool part_read_sector(uchar disk, partition_t* part, uint sector, void* buffer);
bool part_write_sector(uchar disk, partition_t* part, uint sector, void* buffer);

bool part_read_sectors(uchar disk, partition_t* part, uint sector, uint sectors, void* buffer);
bool part_write_sectors(uchar disk, partition_t* part, uint sector, uint sectors, void* buffer);

#endif