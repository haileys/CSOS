#include "kmalloc.h"
#include "stdbool.h"
#include "console.h"
#include "panic.h"

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

	if(chunksz == 0) { // no chunk was big enough, fail to allocate memory
		panic("Could not find a sufficiently large chunk");
		return NULL;
	}
	
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
	
	// first, try to insert the chunk into existing chunks
	bool chunk_returned = false;
	uint freed_chunk = 0;
	for(uint i = 0; i < n_chunks; i++)
	{
		if(!chunks[i].present)
		{
			chunks[i].present = true;
			chunks[i].begin = begin;
			chunks[i].end = end;
			chunk_returned = true;
			freed_chunk = i;
			break;
		}
	}
	
	// if we failed to return the chunk, append it to the end
	if (!chunk_returned) {
		freed_chunk = n_chunks++;
		chunks[freed_chunk].present = true;
		chunks[freed_chunk].begin = begin;
		chunks[freed_chunk].end = end;
	}
	
	while(mm_fix_chunk(freed_chunk, &freed_chunk)) ;
	#ifdef MALLOC_PRINT_DEBUG
		kprintf("freed.\n", len, file, line);
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
