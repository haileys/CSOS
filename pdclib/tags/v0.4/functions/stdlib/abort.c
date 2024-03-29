/* $Id: abort.c 114 2005-12-23 16:04:43Z solar $ */

/* Release $Name$ */

/* abort( void )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <stdlib.h>
#include <signal.h>

#ifndef REGTEST

void abort( void )
{
    raise( SIGABRT );
    exit( 1 );
}

#endif

#ifdef TEST
#include <_PDCLIB_test.h>

#include <stdio.h>

static void aborthandler( int signal )
{
    exit( 0 );
}

int main()
{
    int UNEXPECTED_RETURN_FROM_ABORT = 0;
    BEGIN_TESTS;
    TESTCASE( signal( SIGABRT, &aborthandler ) != SIG_ERR );
    abort();
    TESTCASE( UNEXPECTED_RETURN_FROM_ABORT );
    return TEST_RESULTS;
}

#endif
