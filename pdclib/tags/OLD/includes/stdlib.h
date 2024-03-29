/* ----------------------------------------------------------------------------
 * $Id: stdlib.h 36 2004-06-17 04:46:43Z solar $
 * ----------------------------------------------------------------------------
 * Public Domain C Library - http://pdclib.sourceforge.net
 * This code is Public Domain. Use, modify, and redistribute at will.
 * ----------------------------------------------------------------------------
 * General utilities
 * --------------------------------------------------------------------------*/

#ifndef _STDLIB_H
#define _STDLIB_H _STDLIB_H

#ifndef _NULL
#include "__intern.h"
#endif /* _NULL */

/* TODO: Documentation */

/* ----------------------------------------------------------------------------
 * MACROS
 * --------------------------------------------------------------------------*/

#define NULL _NULL

#define EXIT_FAILURE /* TODO */
#define EXIT_SUCCESS /* TODO */
#define MB_CUR_MAX   /* TODO */
#define RAND_MAX     /* TODO */

/* ----------------------------------------------------------------------------
 * TYPEDEFS
 * --------------------------------------------------------------------------*/

typedef struct
{
    int quotient;
    int remainder;
} div_t;

typedef struct
{
    long quotient;
    long remainder;
} ldiv_t;

typedef struct
{   long long quotient;
    long long remainder;
} lldiv_t;

#ifndef _SIZE_T
#define _SIZE_T _SIZE_T
typedef __size_t size_t
#endif /* _SIZE_T */

#ifndef _WCHAR_T
#define _WCHAR_T _WCHAR_T
typedef __wchar_t wchar_t
#endif /* _WCHAR_T */

/* ----------------------------------------------------------------------------
 * FUNCTIONS
 * --------------------------------------------------------------------------*/

int abs( int i );
long long llabs( long long i );
long labs( long i );

div_t div( int numer, int denom );
lldiv_t lldiv( long long numer, long long denom );
ldiv_t ldiv( long numer, long denom );

int rand( void );
void srand( unsigned int seed );

double atof( const char * s );
int atoi( const char * s );
long atol( const char * s );
long long atoll( const char * s);

double strtod( const char * restrict s, char * * restrict endptr );
float strtof( const char * restrict s, char * * restrict endptr );
long double strtold( const char * restrict s, char * * restrict endptr );

long long strtoll( const char * restrict s, char * * restrict endptr, int base );
unsigned long long strtoull( const char * restrict s, char * * restrict endptr, int base);

long strtol( const char * restrict s, char * * restrict endptr, int base );
unsigned long strtoul( const char * restrict s, char * * restrict endptr, int base);

void * calloc( size_t nelem, size_t size );
void free( void * ptr );
void * malloc( size_t size );
void * realloc( void * ptr, size_t size );

int mblen( const char * s, size_t n );
size_t mbstowcs( wchar_t * restrict wcs, const char * restrict s, size_t n );
int mbtowc( wchar_t * restrict pwc, const char * restrict s, size_t n );
size_t wcstombs( char * restrict s, const wchar_t * restrict wcs, size_t n );
int wctomb( char * s, wchar_t wchar );

void _Exit( int status );
void exit( int status );
void abort( void );
char * getenv( const char * name );
int system( const char * s );

int atexit( void (*func) ( void ) );
void * bsearch( const void * key, const void * base, size_t nelem, size_t size, int (*cmp) ( const void * ck, const void * ce) );
void qsort( void * base, size_t nelem, size_t size, int (*cmp) ( const void * e1, const void * e2) );

#endif /* _STDLIB_H */
