#include "fs/fat.h"
#include "fs/vfs.h"
#include "part.h"
#include "string.h"
#include "console.h"
#include "kmalloc.h"
#include "stdbool.h"

struct fat_vfs_state
{
	vfs_provider_t* prov;
	fat_volume_t volume;
};

//#define KDEBUG(x...) { kprintf("%s:%d: ", __FILE__, __LINE__, __func__); kprintf(x); }
#define KDEBUG(x...) {}

bool fat_vfs_exists(struct fat_vfs_state* state, char* path);
vfs_type_t fat_vfs_type(struct fat_vfs_state* state, char* path);
size_t fat_vfs_size(struct fat_vfs_state* state, char* path);
uint fat_vfs_readdir(struct fat_vfs_state* state, char* path, void* cbstate, bool(*callback)(void*, char*, vfs_type_t));
uint fat_vfs_readfile(struct fat_vfs_state* state, char* path, uint offset, size_t length, void* buffer);

static void fat_read_cached(struct fat_vfs_state* state)
{
	part_read_sectors(state->volume.bios_drive, state->volume.partition, state->volume.first_fat_sector, state->volume.bpb.sectors_per_fat, state->volume.fat);
	
	part_read_sectors(state->volume.bios_drive, state->volume.partition, state->volume.first_data_sector, state->volume.bpb.root_entry_count / (512/32), state->volume.root_entries);
}

vfs_provider_t* fat_vfs(uchar bios_drive, partition_t* partition)
{
    KDEBUG("Entered fat_vfs\n");

	vfs_provider_t* prov = (vfs_provider_t*)kmalloc(sizeof(vfs_provider_t));
	struct fat_vfs_state* state = (struct fat_vfs_state*)kmalloc(sizeof(struct fat_vfs_state));
	prov->state = state;
	state->prov = prov;
	partition_t* part = (partition_t*)kmalloc(sizeof(partition_t));
	memcpy(part, partition, sizeof(partition_t));

    KDEBUG("Past malloc() calls\n");
	fat_init(bios_drive, part, &state->volume);
    KDEBUG("Returned from fat_init()\n");

    KDEBUG("Setting state struct\n");
	state->volume.root_entries = (fat_entry_t*)kmalloc(32 * state->volume.bpb.root_entry_count);
	state->volume.fat = (short*)kmalloc(512 * state->volume.bpb.sectors_per_fat);

    KDEBUG("About to read\n");
	fat_read_cached(state);
	
    KDEBUG("Setting prov struct\n");
	prov->fs_name = "FAT16";
	prov->exists = (bool(*)(void*,char*))fat_vfs_exists;
	prov->type = (vfs_type_t(*)(void*,char*))fat_vfs_type;
	prov->size = (size_t(*)(void*,char*))fat_vfs_size;
	prov->readdir = (uint(*)(void*, char*, void*, bool(*)(void*, char*, vfs_type_t)))fat_vfs_readdir;
	prov->readfile = (uint(*)(void*, char*, uint, size_t, void*))fat_vfs_readfile;
	
	return prov;
}

void fat_init(uchar bios_drive, partition_t* partition, fat_volume_t* volume)
{
	volume->bios_drive = bios_drive;
	volume->partition = partition;
	
	part_read_sector(bios_drive, partition, 0, &volume->bpb);
	
	memcpy(&volume->label, volume->bpb.label, 11);
	volume->label[11] = 0;
	
	volume->root_dir_sectors = ((volume->bpb.root_entry_count * 32) + 511) / 512;
	volume->first_data_sector = volume->bpb.reserved_sectors + (volume->bpb.fat_count * volume->bpb.sectors_per_fat);
	volume->first_fat_sector = volume->bpb.reserved_sectors;
	volume->data_sectors = volume->bpb.sector_count - (volume->bpb.reserved_sectors + (volume->bpb.fat_count * volume->bpb.sectors_per_fat) + volume->root_dir_sectors);
	volume->total_clusters = volume->data_sectors / volume->bpb.sectors_per_cluster;
}

static char* strtofat(char* dest, char* src)
{
	for(uint i = 0; i < 11; i++)
		dest[i] = ' ';
	dest[11] = 0;
	uint len = strlen(src);
	for(uint i = 0, j = 0; j < 11 && i < len && src[i] != 0; i++, j++)
	{
		if(src[i] == '.' && i < 8)
		{
			j = 7;
			continue;
		}
		if(src[i] >= 'a' && src[i] <= 'z')
			dest[j] = 'A' + (src[i] - 'a');
		else
			dest[j] = src[i];
	}
	return dest;
}

static char* fattostr(char* dest, char* src)
{
	uint spaces = 0;
	uint i, j;
	for(i = 0, j = 0; i < 8; i++)
	{
		if(src[i] == ' ')
			spaces++;
		else
		{
			if(spaces > 0)
			{
				while(j < i)
					dest[j++] = ' ';
				spaces = 0;
			}
			if(src[i] >= 'A' && src[i] <= 'Z')
				dest[j] = 'a' + (src[i] - 'A');
			else
				dest[j] = src[i];
			j++;
		}
	}
	dest[j++] = '.';
	while(i < 11 && src[i] != ' ')
	{	
		if(src[i] >= 'A' && src[i] <= 'Z')
			dest[j] = 'a' + (src[i] - 'A');
		else
			dest[j] = src[i];
		j++;
		i++;
	}
	if(dest[j-1] == '.')
		j--;
	dest[j] = 0;
	return dest;
}

ushort fat_read_cluster(fat_volume_t* volume, ushort cluster, void* buffer)
{
	part_read_sectors(volume->bios_drive, volume->partition, volume->first_data_sector + (volume->bpb.root_entry_count / 16) + (cluster * volume->bpb.sectors_per_cluster) - 2, volume->bpb.sectors_per_cluster, buffer);
	return volume->fat[cluster];
}

static fat_entry_t* fat_find_in_dir(fat_entry_t* dir, char* filename)
{
	char fatname[12];
	strtofat(fatname, filename);
	while(dir->filename[0] > 0)
	{
		if(dir->attributes & (FAT_ATTR_DEVICE | FAT_ATTR_VOLUME_ID | FAT_ATTR_UNUSED))
		{	
			dir++;
			continue;
		}
		if(dir->filename[0] == 0xe5)
		{
			dir++;
			continue;
		}
		if(memcmp(fatname, dir->filename, 11) == 0)
		{
			return dir;
		}
		dir++;
	}
	return NULL;
}

static uint parse_path(char* path, char*** path_parts)
{
	char tmp[256];
	uint j = 0;
	uint len = strlen(path);
	uint slashes = 0;
	for(uint i = 0; i < len; i++)
	{
		if(path[i] == '/')
			slashes++;
	}
	char** pathbits = (char**)kmalloc(sizeof(char*) * (slashes + 1));
	uint pathi = 0;
	for(uint i = 0; i <= len; i++)
	{
		if(path[i] != '/' && path[i] != 0)
		{
			tmp[j++] = path[i];
			continue;
		}
		
		if(j == 0)
			continue;
		tmp[j] = 0;
		if(strcmp(tmp, "..") == 0)
		{
			j = 0;
			if(pathi > 0)
			{
				pathi--;
				kfree(pathbits[pathi]);
				pathbits[pathi] = NULL;
			}
			continue;
		}
		if(strcmp(tmp, ".") == 0)
		{
			j = 0;
			continue;
		}
			
		pathbits[pathi] = (char*)kmalloc(j + 1);
		strcpy(pathbits[pathi], tmp);
		pathi++;
		j = 0;
	}
	
	*path_parts = pathbits;
	return pathi;
}

static bool fat_find_ent(struct fat_vfs_state* state, char** path_parts, uint len, fat_entry_t* output)
{	
	fat_entry_t entries[state->volume.bpb.sectors_per_cluster * 16];
	fat_entry_t* dir = state->volume.root_entries;
	for(uint i = 0; i < len; i++)
	{
		fat_entry_t* ent = fat_find_in_dir(dir, path_parts[i]);
		if(ent == NULL)
		{
			return false;
		}
		else
		{
			memcpy(output, ent, sizeof(fat_entry_t));
		}
		if(i + 1 < len)
		{
			if(ent->attributes & FAT_ATTR_DIRECTORY)
			{
				memset(entries, 0, state->volume.bpb.sectors_per_cluster * 16);
				fat_read_cluster(&state->volume, ent->first_cluster, entries);
				dir = entries;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

vfs_type_t fat_vfs_type(struct fat_vfs_state* state, char* path)
{
	char** path_parts;
	uint len = parse_path(path, &path_parts);
	
	vfs_type_t type = VFS_NON_EXISTENT;
	if(len == 0)
	{
		type = VFS_DIR;
	}
	else
	{
		fat_entry_t ent;
		if(fat_find_ent(state, path_parts, len, &ent))
		{
			type = ent.attributes & FAT_ATTR_DIRECTORY ? VFS_DIR : VFS_FILE;
		}
	}
	
	for(uint i = 0; i < len; i++)
		kfree(path_parts[i]);
	kfree(path_parts);
	return type;
}

bool fat_vfs_exists(struct fat_vfs_state* state, char* path)
{
	return fat_vfs_type(state, path) != VFS_NON_EXISTENT;
}

size_t fat_vfs_size(struct fat_vfs_state* state, char* path)
{
	char** path_parts;
	uint len = parse_path(path, &path_parts);
	
	size_t sz = 0;
	if(len == 0)
	{
		sz = 0;
	}
	else
	{
		fat_entry_t ent;
		if(fat_find_ent(state, path_parts, len, &ent))
		{
			sz = ent.size;
		}
	}
	
	for(uint i = 0; i < len; i++)
		kfree(path_parts[i]);
	kfree(path_parts);
	return sz;
}

uint fat_vfs_readdir(struct fat_vfs_state* state, char* path, void* cbstate, bool(*callback)(void*, char*, vfs_type_t))
{
	char** path_parts;
	uint len = parse_path(path, &path_parts);
	uint cnt = 0;
	
	if(len == 0)
	{
		fat_entry_t* dir = state->volume.root_entries;
		for(uint i = 0; i < state->volume.bpb.root_entry_count; i++)
		{
			if(dir[i].filename[0] == 0x00)
				break;
					
			if(dir[i].filename[0] == 0xe5)
				continue;
				
			if(dir[i].attributes & (FAT_ATTR_DEVICE | FAT_ATTR_VOLUME_ID | FAT_ATTR_UNUSED))
				continue;
			
			char tmp[14];
			fattostr(tmp, (char*)dir[i].filename);
			cnt++;
			if(!callback(cbstate, tmp, dir[i].attributes & FAT_ATTR_DIRECTORY ? VFS_DIR : VFS_FILE))
				goto cleanup;
		}
	}
	else
	{
		fat_entry_t ent;
		if(fat_find_ent(state, path_parts, len, &ent))
		{
			if(!(ent.attributes & FAT_ATTR_DIRECTORY))
			{
				goto cleanup;
			}
			fat_entry_t dir[state->volume.bpb.sectors_per_cluster * 16];
			memset(dir, 0, state->volume.bpb.sectors_per_cluster * 512);
			fat_read_cluster(&state->volume, ent.first_cluster, dir);
			fat_entry_t* entp = dir;
			while(entp->filename[0])
			{
				if(entp->filename[0] == 0xe5)
				{
					entp++;
					continue;
				}
					
				if(entp->attributes & (FAT_ATTR_DEVICE | FAT_ATTR_VOLUME_ID | FAT_ATTR_UNUSED))
				{
					entp++;
					continue;
				}
					
				char tmp[14];
				fattostr(tmp, (char*)entp->filename);
				cnt++;
				if(!callback(cbstate, tmp, entp->attributes & FAT_ATTR_DIRECTORY ? VFS_DIR : VFS_FILE))
					goto cleanup;
				entp++;
			}
		}
	}
	
	cleanup:
	for(uint i = 0; i < len; i++)
		kfree(path_parts[i]);
	kfree(path_parts);
	return cnt;
}

static void fat_ent_readcluster(struct fat_vfs_state* state, fat_entry_t* ent, uint cluster, void* buffer)
{
	ushort cluster_number = ent->first_cluster;
	for(uint i = 0; i < cluster; i++)
	{
		// skip ahead clusters
		cluster_number = state->volume.fat[cluster_number];
	}
	
	fat_read_cluster(&state->volume, cluster_number, buffer);
}

uint fat_vfs_readfile(struct fat_vfs_state* state, char* path, uint offset, size_t length, void* buffer)
{
	uint filesize = fat_vfs_size(state, path);
	if(filesize == 0 || length == 0)
		return 0;
	
	char** path_parts;
	uint len = parse_path(path, &path_parts);	
	uint bytes = 0;
	
	fat_entry_t ent;
	if(fat_find_ent(state, path_parts, len, &ent))
	{
		uint bpc = state->volume.bpb.sectors_per_cluster * 512;
		uint start_cluster = offset / bpc;
		uint end_cluster = (offset + length - 1) / bpc;
		char buff[bpc * (end_cluster - start_cluster + 1)];
		for(uint clus = start_cluster; clus <= end_cluster; clus++)
			fat_ent_readcluster(state, &ent, clus, buff + bpc * (clus - start_cluster));
		memcpy(buffer, buff + offset - start_cluster * bpc, length);
		bytes = length;
	}
	
	for(uint i = 0; i < len; i++)
		kfree(path_parts[i]);
	kfree(path_parts);
	return bytes;
}
