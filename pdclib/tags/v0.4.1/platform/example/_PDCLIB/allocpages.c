/* $Id: allocpages.c 262 2006-11-16 07:34:57Z solar $ */

/* Release $Name$ */

/* _PDCLIB_allocpages( int const )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

/* This is an example implementation of _PDCLIB_allocpages() (declared in
   _PDCLIB_config.h), fit for use with POSIX kernels.
*/

#include <stdint.h>
#include <stddef.h>

#ifndef _PDCLIB_CONFIG_H
#define _PDCLIB_CONFIG_H _PDCLIB_CONFIG_H
#include <_PDCLIB_config.h>
#endif

int brk( void * );
void * sbrk( intptr_t );

static void * membreak = NULL;

void * _PDCLIB_allocpages( int const n )
{
    if ( membreak == NULL )
    {
        /* first call, make sure end-of-heap is page-aligned */
        intptr_t unaligned = 0;
        membreak = sbrk( 0 );
        unaligned = _PDCLIB_PAGESIZE - (intptr_t)membreak % _PDCLIB_PAGESIZE;
        if ( unaligned < _PDCLIB_PAGESIZE )
        {
            /* end-of-heap not page-aligned - adjust */
            if ( sbrk( unaligned ) != membreak )
            {
                /* error */
                return NULL;
            }
            membreak = (char *)membreak + unaligned;
        }
    }
    /* increasing or decreasing heap - standard operation */
    void * oldbreak = membreak;
    membreak = (void *)( (char *)membreak + ( n * _PDCLIB_PAGESIZE ) );
    if ( brk( membreak ) == 0 )
    {
        /* successful */
        return oldbreak;
    }
    else
    {
        /* out of memory */
        membreak = oldbreak;
        return NULL;
    }
}

#ifdef TEST
#include <_PDCLIB_test.h>

int puts( const char * );

int main()
{
    BEGIN_TESTS;
#ifndef REGTEST
    {
    void * startbreak = sbrk( 0 );
    TESTCASE( _PDCLIB_allocpages( 0 ) );
    TESTCASE( ( sbrk( 0 ) - startbreak ) <= _PDCLIB_PAGESIZE );
    startbreak = sbrk( 0 );
    TESTCASE( _PDCLIB_allocpages( 1 ) );
    TESTCASE( sbrk( 0 ) == startbreak + ( 1 * _PDCLIB_PAGESIZE ) );
    TESTCASE( _PDCLIB_allocpages( 5 ) );
    TESTCASE( sbrk( 0 ) == startbreak + ( 6 * _PDCLIB_PAGESIZE ) );
    TESTCASE( _PDCLIB_allocpages( -3 ) );
    TESTCASE( sbrk( 0 ) == startbreak + ( 3 * _PDCLIB_PAGESIZE ) );
    }
#endif
    return TEST_RESULTS;
}

#endif

