/* ----------------------------------------------------------------------------
 * $Id: strcmp.c 36 2004-06-17 04:46:43Z solar $
 * ----------------------------------------------------------------------------
 * Public Domain C Library - http://pdclib.sourceforge.net
 * This code is Public Domain. Use, modify, and redistribute at will.
 * --------------------------------------------------------------------------*/

int strcmp( const char * src_1, const char * src_2 )
{
    while ( ( *src_1 != '\0' ) && ( *src_1 == *src_2 ) )
    {
        ++src_1;
        ++src_2;
    }
    return ( *src_1 - *src_2 );
}
