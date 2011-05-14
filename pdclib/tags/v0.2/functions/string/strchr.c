/* $Id: strchr.c 55 2005-01-11 06:42:51Z solar $ */

/* Release $Name$ */

/* strchr( const char *, int )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <string.h>

char * strchr( const char * s, int c )
{
    do
    {
        if ( *s == (char) c )
        {
            return (char *) s;
        }
    } while ( *s++ );
    return NULL;
}

#warning Test driver missing.
