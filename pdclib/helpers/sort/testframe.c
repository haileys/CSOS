#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "testframe.h"

struct testresult rc;

char * stack[ STACKSIZE ];
size_t stackidx = 0;

int compare( const void * x, const void * y )
{
    ++(rc.compares);
    return *( (int *)x ) - *( (int *)y );
}

int array[ ARRAYSIZE ];
int check[ ARRAYSIZE ];

void * GLBL_base;
size_t GLBL_nmemb;
size_t GLBL_size;

static FILE * logfile;

void print_intermediate( )
{
    for ( int i = 0; i < ARRAYSIZE; ++i )
    {
        fprintf( logfile, "%d  ", array[i] );
    }
    fputs( "\n", logfile );
}

void print_pivot_info( int lower, int upper, int pivot, int oldpos )
{
    fprintf( logfile, "In sort from %d to %d, using pivot %d from position %d.\n", lower, upper, pivot, oldpos );
}

void print_pivot_return( char * pivot, char * newpos )
{
    fprintf( logfile, "Returning pivot %d to position %d.\n", *((int *) pivot), *((int *) newpos) );
}


static void sorted( int array[] )
{
    size_t i;
    for ( i = 0; i < ARRAYSIZE; ++i )
    {
        array[ i ] = i;
    }
}

static void reverse( int array[] )
{
    size_t i;
    for ( i = 0; i < ARRAYSIZE; ++i )
    {
        array[ i ] = ARRAYSIZE - i;
    }
}

static void sawtooth( int array[] )
{
    size_t i;
    for ( i = 0; i < ARRAYSIZE; ++i )
    {
        array[ i ] = i % 10;
    }
}

static void bell( int array[] )
{
    size_t i;
    for ( i = 0; i < ARRAYSIZE / 2; ++i )
    {
        array[ i ] = i;
        array[ ARRAYSIZE - ( i + 1 ) ] = i;
    }
}

struct
{
    void (*func)(int[]);
    char * func_name;
} testcases[] = { { sorted, "Already-sorted" },
                  { reverse, "Reverse-sorted" },
                  { sawtooth, "Sawtooth" },
                  { bell, "Bell curve" },
                  { worst_case, "Worst Case" }
                };

void test()
{
    size_t i, j;
    remove( "log.csv" );
    logfile = fopen( "log.csv", "a" );
    if ( logfile == NULL )
    {
        fputs( "Could not open log.csv for output.\n", stderr );
        exit( 1 );
    }
    for ( i = 0; i < sizeof( testcases ) / sizeof( *testcases ); ++i )
    {
        rc.compares = 0;
        rc.swaps = 0;
        rc.recursions = 0;
        rc.curr_recurse = 0;
        testcases[i].func( array );
        fprintf( logfile, "\n----------------------------------------------------------------------\n%s\n", testcases[i].func_name );
        GLBL_base = array;
        GLBL_nmemb = ARRAYSIZE;
        GLBL_size = sizeof( int );
        quicksort( array, ARRAYSIZE, sizeof( int ), compare );
        testcases[i].func( check );
        qsort( check, ARRAYSIZE, sizeof( int ), compare );
        int error = 0;
        for ( j = 0; j < ARRAYSIZE; ++j )
        {
            fprintf( logfile, "%d  ", check[j] );
            if ( compare( &array[j], &check[j] ) )
            {
                error = 1;
            }
        }
        fputs( "\n", logfile );
        if ( error )
        {
            fputs( "ERROR!\n", logfile );
        }
    }
    fclose( logfile );
}

void memswp( char * x, char * y, size_t size )
{
    ++(rc.swaps);
    while ( size-- > 0 )
    {
        char tmp = *x;
        *(x++) = *y;
        *(y++) = tmp;
    }
}

