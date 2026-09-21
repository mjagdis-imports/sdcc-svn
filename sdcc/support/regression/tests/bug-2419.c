/*
   bug-2419.c
*/

#include <testfwk.h>

char __xdata c0[] = "123";
char __xdata c1[] = "abc";
char __xdata *gp = c0;

void __xdata *aligned_a (void)
{
  return gp;
}

extern void __xdata *aligned_a (void);

void testBug (void)
{
  ASSERT (aligned_a () == c0);
  gp = c1;
  ASSERT (aligned_a () == c1);
}

