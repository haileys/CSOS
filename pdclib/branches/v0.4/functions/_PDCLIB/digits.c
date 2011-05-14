/* $Id: digits.c 254 2006-07-28 05:49:00Z solar $ */

/* Release $Name$ */

/* _PDCLIB_digits

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

char _PDCLIB_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

#ifdef TEST
#include <_PDCLIB_test.h>

int main()
{
    BEGIN_TESTS;
    /* no tests for raw data */
    return TEST_RESULTS;
}

#endif
