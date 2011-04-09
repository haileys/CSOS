#ifndef VFS_H
#define VFS_H

#include "stddef.h"
#include "stdbool.h"

typedef enum vfs_type
{
	VFS_NON_EXISTENT = 0x00,
	VFS_FILE = 0x01,
	VFS_DIR = 0x02,
}
vfs_type_t;

typedef enum vfs_seek
{
	VFS_SEEK_SET,
	VFS_SEEK_CUR,
	VFS_SEEK_END,
}
vfs_seek_t;

typedef struct vfs_stream
{
	void* state;
	void(*seek)(void*, int /* signed offset */, vfs_seek_t);
	uint(*read)(void*, uint /* len */, void* /* buffer */);
	uint(*write)(void*, uint /* len */, void* /* buffer */);
	void(*close)(void*);
}
vfs_stream_t;

typedef struct vfs_provider
{
	void* state;
	char* fs_name;
	void(*unmount)(void*);
	bool(*exists)(void*, char*);
	vfs_type_t(*type)(void*, char*);
	size_t(*size)(void*, char*);
	uint(*readdir)(void*, char*, void*, bool(*)(void*, char*, vfs_type_t));
	uint(*readfile)(void*, char*, uint, size_t, void*);
}
vfs_provider_t;

void vfs_init(vfs_provider_t* root_fs);
bool vfs_exists(char* path);
vfs_type_t vfs_type(char* path);
size_t vfs_size(char* path);
vfs_stream_t* vfs_open(char* path);
void vfs_close(vfs_stream_t* stream);
uint vfs_readdir(char* dir, void* state, bool(*callback)(void*, char*, vfs_type_t));
uint vfs_readfile(char* path, uint offset, size_t length, void* buffer);

#endif