/* $Id: strstr.c 55 2005-01-11 06:42:51Z solar $ */

/* Release $Name$ */

/* strstr( const char *, const char * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <string.h>

char * strstr( const char * s1, const char * s2 )
{
    const char * p1 = s1;
    const char * p2;
    while ( *s1 )
    {
        p2 = s2;
        while ( *p2 && ( *p1 == *p2 ) )
        {
            ++p1;
            ++p2;
        }
        if ( ! *p2 )
        {
            return (char *) s1;
        }
        ++s1;
        p1 = s1;
    }
    return NULL;
}

#warning Test driver missing.
