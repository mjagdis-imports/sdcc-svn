/* New tests for some existing z80 peephole optimizer rules that were no longer triggered by their original testcases.
   Tetscases found using Gemma-4-31B-IT-QAT-Q4 */

#include <testfwk.h>

#include <limits.h>

unsigned char f102a(unsigned char x) {
    x++;
    if (x) {
        return 0;
    }
    return 1;
}

void test102a(void)
{
   ASSERT (f102a(0) == 0);
   ASSERT (f102a(UCHAR_MAX) == 1);
}

