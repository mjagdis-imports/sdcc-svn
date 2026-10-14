/* An issue in designated initializers for anonymous unions in structs. */
#include <testfwk.h>

struct s
{
   union
   {
      int a;
      int b;
   };

   int c;
   int d;
};

struct s s2 = { .a = 0, .c = 3, .d = 4 };

void testBug(void)
{
   ASSERT(s2.a == 0);
   ASSERT(s2.c == 3);
   ASSERT(s2.d == 4);
}

