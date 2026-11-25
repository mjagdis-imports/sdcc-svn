/* bug-3855.c
   An issue in handling of unsigned _BitInt divisions triggered by generalized constant propagation.
 */
 
#include <testfwk.h>

long g(long t) {
    if(t>=2000 && t<4250)
        t=(24*t-38580)/1000;
    return t;
}

void testBug(void) {
    long r = g(4000);
    ASSERT( r == 57 ); // 24*4000-38580=57420
}

