#ifndef FAT_H
#define FAT_H

typedef struct fat_bpb
{
	char jmp_short[3];
	char oem[8];
	ushort bytes_per_sector;
	uchar sectors_per_cluster;
	ushort reserved_sectors;
	uchar fat_count;
	ushort directory_entry_count;
	ushort sectors;
	uchar media_descriptor;
	ushort sectors_per_fat;
	ushort sectors_per_track;
	ushort heads;
	uint hidden_sectors;
	uint sectors_large;
	
	// ebpb
	uchar drive_number;
	uchar nt_flags;
	uchar signature;
	uint volume_serial;
	uchar sys_ident_str[8];
	uchar boot_code[448];
	uchar boot_signature[2];
}
__attribute__((__packed__)) fat_bpb_t;



#endif