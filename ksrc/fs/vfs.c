#include "fs/vfs.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"
#include "kmalloc.h"

vfs_provider_t* root;

void vfs_init(vfs_provider_t* root_fs)
{
	root = root_fs;
}

bool vfs_exists(char* path)
{
	return root->exists(root->state, path);
}

vfs_type_t vfs_type(char* path)
{
	return root->type(root->state, path);
}

size_t vfs_size(char* path)
{
	return root->size(root->state, path);
}

uint vfs_readdir(char* dir, void* state, bool(*callback)(void*, char*, vfs_type_t))
{
	return root->readdir(root->state, dir, state, callback);
}

uint vfs_readfile(char* path, uint offset, size_t length, void* buffer)
{
	return root->readfile(root->state, path, offset, length, buffer);
}