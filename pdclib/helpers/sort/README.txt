This is a test frame for sorting functions, written during the work on the Public Domain C Library
(PDCLib). The idea is to have one source file for each algorithm, which includes "testframe.h" and:

- provides a function with the name "quicksort" and the same paramter list as standard qsort()
  quicksort( void * base, size_t nmemb, size_t size, int (*compar)( void *, void * ) )
  which...

- calls the function memswp( char * x, char * y, size_t size ) to swap two elements,

- if recursive, uses RECURSE( base, nmemb, size, compar ) instead of quicksort( ... ) to recurse,

- calls RETURN() instead of return() if returning from a recursion,

- if using an internal stack, calls PUSH( lower, upper ) and POP( lower, upper ) instead of any own
  stack functions,

- provides a function worst_case( int array[ ARRAYSIZE ] ) that fills array[] with a worst-case
  scenario for the sorting function.

Compiling this source file together with testframe.c will yield an executable that, when run, does
perform a series of tests using various inputs on the provided quicksort() function, including the
worst-case scenario provided by the implementor. The output will list, for each such test:

- the number of comparisons performed,
- the number of swaps performed,
- the maximum recursion depth,
- the maximum stack use.

This provides a valuable benchmark when comparing algorithms. It also serves to test the algorithm
for correctness, as each test is also run with the standard qsort() and the two results compared.
