/* ----------------------------------------------------------------------------
 * $Id: ungetc.c 36 2004-06-17 04:46:43Z solar $
 * ----------------------------------------------------------------------------
 * Public Domain C Library - http://pdclib.sourceforge.net
 * This code is Public Domain. Use, modify, and redistribute at will.
 * --------------------------------------------------------------------------*/

int ungetc( int c, FILE * stream ) { /* TODO */ };

/* PDPC code - unreviewed
{
    if ((stream->ungetCh != -1) || (c == EOF))
    {
        return (EOF);
    }
    stream->ungetCh = (unsigned char)c;
    stream->quickText = 0;
    return ((unsigned char)c);
}
*/