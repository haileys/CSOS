/* ----------------------------------------------------------------------------
 * $Id: wcstombs.c 36 2004-06-17 04:46:43Z solar $
 * ----------------------------------------------------------------------------
 * Public Domain C Library - http://pdclib.sourceforge.net
 * This code is Public Domain. Use, modify, and redistribute at will.
 * --------------------------------------------------------------------------*/

size_t wcstombs( char * restrict s, const wchar_t * restrict wcs, size_t n ) { /* TODO */ };

/* PDPC code - unreviewed
{
    strncpy(s, (const char *)pwcs, n);
    if (strlen((const char *)pwcs) >= n)
    {
        return (n);
    }
    return (strlen(s));
}
*/
