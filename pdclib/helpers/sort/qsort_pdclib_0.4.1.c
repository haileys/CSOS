#include "testframe.h"

#define T 7

void quicksort( void * base, size_t nmemb, size_t size, int (*compar)( const void *, const void * ) )
{
    char * i;
    char * j;
    size_t thresh     = T * size;
    char * base_      = (char *)base;
    char * limit      = base_ + nmemb * size;

    if ( ( nmemb == 0 ) || ( size == 0 ) || ( base == NULL ) )
    {
        return;
    }

    for ( ;; )
    {
        print_intermediate();
        if ( limit - base_ > thresh ) /* QSort for more than T elements. */
        {
            /* We work from second to last - first will be pivot element. */
            i = base_ + size;
            j = limit - size;
            /* We swap first with middle element, then sort that with second
            and last element so that eventually first element is the median
            of the three - avoiding pathological pivots.
            TODO: Instead of middle element, chose one randomly.
            */
            memswp( ( ( ( (size_t)( limit - base_ ) ) / size ) / 2 ) * size + base_, base_, size );
            if ( compar( i, j ) > 0 ) memswp( i, j, size );
            if ( compar( base_, j ) > 0 ) memswp( base_, j, size );
            if ( compar( i, base_ ) > 0 ) memswp( i, base_, size );
            /* Now we have the median for pivot element, entering main Quicksort. */
            for ( ;; )
            {
                do
                {
                    /* move i right until *i >= pivot */
                    i += size;
                } while ( compar( i, base_ ) < 0 );
                do
                {
                    /* move j left until *j <= pivot */
                    j -= size;
                } while ( compar( j, base_ ) > 0 );
                if ( i > j )
                {
                    /* break loop if pointers crossed */
                    break;
                }
                /* else swap elements, keep scanning */
                memswp( i, j, size );
            }
            /* move pivot into correct place */
            memswp( base_, j, size );
            /* recurse into larger subpartition, iterate on smaller */
            if ( j - base_ > limit - i )
            {
                /* left is larger */
                RECURSE( base, ( j - base_ ) / size, size, compar );
                base_ = i;
            }
            else
            {
                /* right is larger */
                RECURSE( base, ( limit -i ) / size, size, compar );
                limit = j;
            }
        }
        else /* insertion sort for less than T elements              */
        {
            for ( j = base_, i = j + size; i < limit; j = i, i += size )
            {
                for ( ; compar( j, j + size ) > 0; j -= size )
                {
                    memswp( j, j + size, size );
                    if ( j == base_ )
                    {
                        break;
                    }
                }
            }
            break;
        }
    }
    RETURN;
}

void worst_case( int array[] )
{
    size_t i;
    for ( i = 0; i < ARRAYSIZE; ++i )
    {
        array[ ARRAYSIZE ] = i;
    }
}

int main()
{
    test();
}
