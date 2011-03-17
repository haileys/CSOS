#ifndef FAT_H
#define FAT_H

#include "stddef.h"
#include "stdbool.h"
#include "part.h"
#include "fs/vfs.h"

typedef struct fat_bpb
{
	uchar jmp_short[3];
	uchar oem[8];
	ushort bytes_per_sector;
	uchar sectors_per_cluster;
	ushort reserved_sectors;
	uchar fat_count;
	ushort root_entry_count;
	ushort sector_count;
	uchar media_descriptor;
	ushort sectors_per_fat;
	ushort sectors_per_track;
	ushort heads;
	uint hidden_sector_count;
	uint long_sector_count;
	
	// ebpb
	uchar drive_number;
	uchar nt_flags;
	uchar signature;
	uint serial;
	uchar label[11];
	uchar sys_ident[8];
	uchar boot_code[450];
}
__attribute__((__packed__)) fat_bpb_t;

typedef enum fat_attributes
{
	FAT_ATTR_READ_ONLY = 0x01,
	FAT_ATTR_HIDDEN = 0x02,
	FAT_ATTR_SYSTEM = 0x04,
	FAT_ATTR_VOLUME_ID = 0x08,
	FAT_ATTR_DIRECTORY = 0x10,
	FAT_ATTR_ARCHIVE = 0x20,
	FAT_ATTR_DEVICE = 0x40,
	FAT_ATTR_UNUSED = 0x80,
}
fat_attributes_t;

typedef struct fat_entry
{
	uchar filename[11];
	uchar attributes;
	uchar nt_reserved;
	uchar creation_tenths_second;
	ushort creation_hms;
	ushort creation_ymd;
	ushort access_ymd;
	ushort first_cluster_high;
	ushort modified_hms;
	ushort modified_ymd;
	ushort first_cluster;
	uint size;
}
__attribute__((__packed__)) fat_entry_t;

typedef struct fat_volume
{
	uchar bios_drive;
	partition_t* partition;
	
	short* fat;
	fat_entry_t* root_entries;
	
	uint root_dir_sectors;
	uint first_data_sector;
	uint first_fat_sector;
	uint data_sectors;
	uint total_clusters;
	
	char label[12];
	
	fat_bpb_t bpb;
}
fat_volume_t;

vfs_provider_t* fat_vfs(uchar bios_drive, partition_t* partition);
void fat_init(uchar bios_drive, partition_t* partition, fat_volume_t* volume);
ushort fat_read_cluster(fat_volume_t* volume, ushort cluster, void* buffer);

#endif