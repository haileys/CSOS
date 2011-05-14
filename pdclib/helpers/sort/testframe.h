/* ------------------------------------------------------------------------- */
/* Sort algorithm benchmarking framework                                     */
/* ------------------------------------------------------------------------- */
/* HISTORY                                                                   */
/* When it came to implementing the qsort() function for PDCLib, some doubts */
/* were voiced about the correctness and performance of the implementation I */
/* used in the v0.4 release. I realized that it would be necessary to prove  */
/* both by more elaborate testing than currently done by the test driver, to */
/* satisfy my users (and myself). After some fiddling with ad-hoc testing, I */
/* decided to create a more elaborate framework for testing and proving any  */
/* algorithms and mutations thereof that might be candidates for qsort().    */
/* ------------------------------------------------------------------------- */
/* USAGE                                                                     */
/* The framework provides several features to thoroughly test your sorting   */
/* algorithm. The idea is to create a separate source file, where you:       */
/* - include this header file;                                               */
/* - implement your algorithm using the quicksort() function prototype;      */
/* - use the memswp() function to swap elements in the to-be-sorted array;   */
/* - if you work with an internal stack, use the PUSH and POP macros;        */
/* - if you work recursively, use the RECURSE macro, and call RETURN to exit */
/*   the function;                                                           */
/* - determine what would be the worst case for your algorithm and implement */
/*   the worst_case() function in a way that it fills array[ARRAYSIZE] with  */
/*   a scenario of that worst case (if you don't know what the worst case    */
/*   for your algorithm is, don't bother to benchmark it - you have not even */
/*   fully understood it, so go back to the drawing board);                  */
/* - call the run_test() function.                                           */
/* The framework will automatically benchmark your algorthm, and print the   */
/* results, including the number of comparisons, number of swaps, and the    */
/* deepest recursion / highest stack use for a variety of inputs.            */
/* ------------------------------------------------------------------------- */

#include <stddef.h>

/* --------------------- BEGIN user-configurable part ---------------------- */

/* Adjust this to run the tests on inputs of a different size. You should    */
/* run all benchmarks of a test series with identical ARRAYSIZE, obviously.  */
#define ARRAYSIZE 20

/* Should your sorting method require an extraordinate amount of stack, you  */
/* can extend this value to suit your algorithm. (But doesn't the stack      */
/* requirement disqualify it right away?)                                    */
#define STACKSIZE 1000

/* ---------------------- END user-configurable part ----------------------- */

/* -------------------- BEGIN user-implementation part --------------------- */

void worst_case( int array[] );
void quicksort( void * base, size_t nmemb, size_t size, int (*compar)( const void *, const void * ) );

/* --------------------- END user-implementation part ---------------------- */

/* --------------- No user-servicable parts after this line ---------------- */

struct testresult
{
    unsigned long long compares;
    unsigned long long swaps;
    unsigned long long recursions;
    unsigned long long curr_recurse;
    unsigned long long stackuse;
};

#define MAX( x, y ) (x) > (y) ? (x) : (y)
#define RECURSE( base, nmemb, size, compar ) do { ++(rc.curr_recurse); \
        rc.recursions = rc.recursions > rc.curr_recurse ? \
                        rc.recursions : rc.curr_recurse; \
        quicksort( base, nmemb, size, compar ); } while ( 0 )
#define RETURN --(rc.curr_recurse);

#define PUSH( lower, upper ) do { stack[stackidx++] = lower; \
        stack[stackidx++] = upper; assert( stackidx < STACKSIZE ); \
        rc.stackuse = MAX( rc.curr_stackuse, stackidx / 2 ); } while (0)
#define POP( lower, upper ) do { assert( stackidx >= 2 ); \
        assert( ! ( stackidx % 2 ) ); upper = stack[--stackidx]; \
        lower = stack[--stackidx]; } while (0)

int compare( const void *, const void * );
void memswp( char * x, char * y, size_t size );
extern struct testresult rc;
extern int array[];

void test();
void print_intermediate();
void print_pivot_info( int lower, int upper, int pivot, int oldpos );
void print_pivot_return( char *, char * );

