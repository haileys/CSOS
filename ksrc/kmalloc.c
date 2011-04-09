#include "kmalloc.h"
#include "stdbool.h"
#include "console.h"

static char* alloc_begin = NULL;
static char* alloc_end = NULL;

struct free_chunk
{
	bool present;
	char* begin;
	char* end;
};

static struct free_chunk chunks[2048];
static uint n_chunks;

static bool mm_fix_chunk(uint chunk, uint* next_chunk);

void kmalloc_init(char* begin, size_t len)
{
	alloc_begin = begin;
	alloc_end = begin + len;
	chunks[0].begin = alloc_begin;
	chunks[0].end = alloc_end;
	chunks[0].present = true;
	n_chunks = 1;
}

#ifdef MALLOC_PRINT_DEBUG
void* _kmalloc(size_t len, char* file, uint line)
#else
void* _kmalloc(size_t len)
#endif
{
	uint chunk;
	uint chunksz = 0;
	for(uint i = 0; i < n_chunks; i++)
	{
		if(!chunks[i].present)
			continue;
			
		size_t chunksize = chunks[i].end - chunks[i].begin;
		if(chunksize < len + 4)
			continue;
		if(chunksz == 0 || chunksize < chunksz)
		{
			chunk = i;
			chunksz = chunksize;
		}	
		if(chunksize == len + 4)
			break;
	}
	if(chunksz == 0) // no chunk was big enough, fail to allocate memory
		return NULL;
	
	// found a chunk to allocate from
	uint* sz = (uint*)chunks[chunk].begin;
	chunks[chunk].begin += len + 4;
	if(chunks[chunk].end - chunks[chunk].begin == 0)
		chunks[chunk].present = false;
	
	*sz++ = len;
	#ifdef MALLOC_PRINT_DEBUG
		kprintf("Allocated %d bytes in %s:%d\n", len, file, line);
	#endif
	return sz;
}


#ifdef MALLOC_PRINT_DEBUG
void _kfree(void* ptr, char* file, uint line)
#else
void _kfree(void* ptr)
#endif
{
	#ifdef MALLOC_PRINT_DEBUG
		kprintf("Freeing from 0x%x on %s:%d... ", ptr, file, line);
	#endif
	char* begin = ((char*)ptr) - 4;
	size_t len = *(size_t*)begin;
	char* end = begin + len;
	
	uint free_slot = n_chunks++; // if we're going to add a chunk to the end, make sure to increment n_chunks
	for(uint i = 0; i < n_chunks; i++)
	{
		if(!chunks[i].present)
		{
			// if it turns out we WON'T use a chunk at the end, take off that increment we put before
			n_chunks--;
			
			free_slot = i;
			break;
		}
	}
	
	chunks[free_slot].present = true;
	chunks[free_slot].begin = begin;
	chunks[free_slot].end = end;
	
	uint c = free_slot;
	while(mm_fix_chunk(c, &c)) ;
	#ifdef MALLOC_PRINT_DEBUG
		kprintf("freed.\n", len, file, line);
	file = file;
	line = line;
	#endif
}

static bool mm_fix_chunk(uint chunk, uint* next_chunk)
{
	char* begin = chunks[chunk].begin;
	char* end = chunks[chunk].end;
	
	for(uint i = 0; i < n_chunks; i++)
	{
		if(!chunks[i].present)
			continue;
		
		if(chunks[i].begin == end)
		{
			*next_chunk = i;
			chunks[i].begin = begin;
			chunks[chunk].present = false;
			return true;
		}
		
		if(chunks[i].end == begin)
		{
			*next_chunk = i;
			chunks[i].end = end;
			chunks[chunk].present = false;
			return true;
		}
	}
	
	return false;
}