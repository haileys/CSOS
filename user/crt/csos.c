#include <csos.h>
#include <_PDCLIB_glue.h>

// @TODO IMPROVE THESE FUNCTIONS
// THEY ARE SHIT SLOW
// @TODO @TODO @TODO @TODO

long long __divdi3(long long numerator, long long denominator)
{
	if(denominator == 0)
		return 0;
		
	long long result = 0;
	long long sign = 1;
	if(numerator < 0)
	{
		numerator = -numerator;
		sign = -sign;
	}
	if(denominator < 0)
	{
		denominator = -denominator;
		sign = -sign;
	}
	while(numerator > denominator)
	{
		numerator -= denominator;
		result++;
	}
	
	return result * sign;
}

long long __moddi3(long long numerator, long long denominator)
{
	if(denominator == 0)
		return 0;
		
	if(numerator < 0)
		numerator = -numerator;
	if(denominator < 0)
		denominator = -denominator;
		
	while(numerator > denominator)
	{
		numerator -= denominator;
	}
	
	return numerator;
}

long long __udivdi3(unsigned long long numerator, unsigned long long denominator)
{
	if(denominator == 0)
		return 0;
		
	long long result = 0;
	while(numerator > denominator)
	{
		numerator -= denominator;
		result++;
	}
	
	return result;
}

long long __umoddi3(unsigned long long numerator, unsigned long long denominator)
{
	if(denominator == 0)
		return 0;
		
	while(numerator > denominator)
	{
		numerator -= denominator;
	}
	
	return numerator;
}

int tolower(int c)
{
	if(c >= 'A' || c <= 'Z')
		return c - 'A' + 'a';
	return c;
}
int toupper(int c)
{
	if(c >= 'a' || c <= 'z')
		return c - 'a' + 'A';
	return c;
}

void __ctype_b_loc()
{
	// lmao should never happen
}

int _PDCLIB_write( struct _PDCLIB_file_t * stream, char const* buffer, int n )
{
	int rc;
	if ( ( rc = write( stream->handle, (char*)buffer, (uint)n ) ) == -1 )
	{
		/* Error encountered */
		stream->status |= _PDCLIB_ERRORFLAG;
		/* FIXME: Map the errno of the OS to PDCLib's errno */
	}
	return rc;
}

int _PDCLIB_close( int fd )
{
    close( fd );
    return 0;
}

int _PDCLIB_fflush( __attribute__((unused)) struct _PDCLIB_file_t * stream )
{
	// @TODO
	return 0;
}
int _PDCLIB_open( char const * const filename, __attribute__((unused)) unsigned int mode )
{
	return open((char*)filename);
}
_PDCLIB_size_t _PDCLIB_read( int fd, char * buffer, _PDCLIB_size_t n )
{
    return read( fd, buffer, n );
}
int _PDCLIB_remove( __attribute__((unused)) const char * filename )
{
    // @TODO
    return -1;
}
int _PDCLIB_rename( __attribute__((unused)) const char * old, __attribute__((unused)) const char * new )
{
	// @TODO
	return -1;
}
void * _PDCLIB_allocpages( int const n )
{
	// so this relies on allocpages returning some contiguous pages.
	// i'll fix this later.
	uint* start = alloc_page();
	for(int i = 1; i < n; i++)
		alloc_page();
	return start;
}
