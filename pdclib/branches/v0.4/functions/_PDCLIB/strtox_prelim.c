/* $Id: strtox_prelim.c 254 2006-07-28 05:49:00Z solar $ */

/* Release $Name$ */

/* _PDCLIB_strtox_prelim( const char *, char *, int * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <ctype.h>

const char * _PDCLIB_strtox_prelim( const char * p, char * sign, int * base )
{
    /* skipping leading whitespace */
    while ( isspace( *p ) ) ++p;
    /* determining / skipping sign */
    if ( *p != '+' && *p != '-' ) *sign = '+';
    else *sign = *(p++);
    /* determining base */
    if ( *p == '0' )
    {
        ++p;
        if ( ( *base == 0 || *base == 16 ) && ( *p == 'x' || *p == 'X' ) )
        {
            *base = 16;
            ++p;
        }
        else if ( *base == 0 )
        {
            *base = 8;
        }
        else
        {
            --p;
        }
    }
    else if ( ! *base )
    {
        *base = 10;
    }
    return p;
    return ( ( *base >= 2 ) && ( *base <= 36 ) ) ? p : NULL;
}

#ifdef TEST
#include <_PDCLIB_test.h>

int main()
{
    int base = 0;
    char sign = '\0';
    char test1[] = "  123";
    char test2[] = "\t+0123";
    char test3[] = "\v-0x123";
    BEGIN_TESTS;
    TESTCASE( _PDCLIB_strtox_prelim( test1, &sign, &base ) == &test1[2] );
    TESTCASE( sign == '+' );
    TESTCASE( base == 10 );
    base = 0;
    sign = '\0';
    TESTCASE( _PDCLIB_strtox_prelim( test2, &sign, &base ) == &test2[3] );
    TESTCASE( sign == '+' );
    TESTCASE( base == 8 );
    base = 0;
    sign = '\0';
    TESTCASE( _PDCLIB_strtox_prelim( test3, &sign, &base ) == &test3[4] );
    TESTCASE( sign == '-' );
    TESTCASE( base == 16 );
    base = 10;
    sign = '\0';
    TESTCASE( _PDCLIB_strtox_prelim( test3, &sign, &base ) == &test3[2] );
    TESTCASE( sign == '-' );
    TESTCASE( base == 10 );
    return TEST_RESULTS;
}

#endif
