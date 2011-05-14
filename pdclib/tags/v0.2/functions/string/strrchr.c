/* $Id: strrchr.c 58 2005-01-11 07:15:12Z solar $ */

/* Release $Name$ */

/* strrchr( const char *, int c )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <string.h>

char * strrchr( const char * s, int c )
{
    size_t i = 0;
    while ( s[i++] );
    do
    {
        if ( s[--i] == (char) c )
        {
            return (char *) s + i;
        }
    } while ( i );
    return NULL;
}

#warning Test driver missing.
