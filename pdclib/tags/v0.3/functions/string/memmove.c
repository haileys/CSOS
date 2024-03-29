/* $Id: memmove.c 71 2005-11-21 18:08:43Z solar $ */

/* Release $Name$ */

/* memmove( void *, const void *, size_t )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <string.h>

void * memmove( void * s1, const void * s2, size_t n )
{
    char * dest = (char *) s1;
    const char * src = (const char *) s2;
    if ( dest <= src )
    {
        while ( n-- )
        {
            *dest++ = *src++;
        }
    }
    else
    {
        src += n;
        dest += n;
        while ( n-- )
        {
            *--dest = *--src;
        }
    }
    return s1;
}

#ifdef TEST
#include <_PDCLIB_test.h>

int main()
{
    char s[] = "xxxxabcde";
    BEGIN_TESTS;
    TESTCASE( memmove( s, s + 4, 5 ) == s );
    TESTCASE( s[0] == 'a' );
    TESTCASE( s[4] == 'e' );
    TESTCASE( s[5] == 'b' );
    TESTCASE( memmove( s + 4, s, 5 ) == s + 4 );
    TESTCASE( s[4] == 'a' );
    return TEST_RESULTS;
}
#endif
