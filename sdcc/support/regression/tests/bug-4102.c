/* bug-4102.c
   Avoid host signed overflow when folding unsigned int multiplication.
 */
#include <testfwk.h>

void testBug(void)
{
  ASSERT((unsigned int)65535 * (unsigned int)65535 ==
         (unsigned int)4294836225UL);
  ASSERT((unsigned int)65535 * (unsigned int)1 == (unsigned int)65535);
  ASSERT((unsigned int)65535 * (unsigned int)0 == 0);
}
