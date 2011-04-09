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

// vfs stream stuff for normal file systems
struct vfs_stream_state
{
	uint offset;
	char* path;
	vfs_stream_t* stream;
};

static void vfs_stream_seek(void* state, int offset, vfs_seek_t seek)
{
	struct vfs_stream_state* st = (struct vfs_stream_state*)state;
	switch(seek)
	{
		case VFS_SEEK_SET:
			st->offset = offset > 0 ? offset : 0;
			break;
		case VFS_SEEK_CUR:	
			if(offset < 0 && (uint)-offset > st->offset)
				st->offset = 0;
			else
				st->offset += offset;
			break;
		case VFS_SEEK_END:
			st->offset = vfs_size(st->path);
			break;
	}
}

static void vfs_stream_close(void* state)
{
	struct vfs_stream_state* st = (struct vfs_stream_state*)state;
	kfree(st->path);
	kfree(st->stream);
	kfree(state);
}

vfs_stream_t* vfs_open(char* path)
{
	vfs_stream_t* stream = (vfs_stream_t*)kmalloc(sizeof(vfs_stream_t));
	struct vfs_stream_state* state = (struct vfs_stream_state*)kmalloc(sizeof(struct vfs_stream_state));
	stream->state = state;
	stream->seek = vfs_stream_seek;
	stream->read = NULL;
	stream->write = NULL;
	stream->close = vfs_stream_close;
	state->path = kmalloc(strlen(path) + 1);
	strcpy(state->path, path);
	return stream;
}
void vfs_close(vfs_stream_t* stream)
{
	stream->close(stream->state);
}