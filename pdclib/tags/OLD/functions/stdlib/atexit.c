/* ----------------------------------------------------------------------------
 * $Id: atexit.c 36 2004-06-17 04:46:43Z solar $
 * ----------------------------------------------------------------------------
 * Public Domain C Library - http://pdclib.sourceforge.net
 * This code is Public Domain. Use, modify, and redistribute at will.
 * --------------------------------------------------------------------------*/

int atexit( void (*func) ( void ) ) { /* TODO */ };

/* PDPC code - unreviewed
{
    int x;
    
    for (x = 0; x < __NATEXIT; x++)
    {
        if (__userExit[x] == 0)
        {
            __userExit[x] = func;
            return (0);
        }
    }
    return (-1);
}
*/
