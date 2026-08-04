/* PR tree-optimization/103255 */

/* Suppress valid warnings:
warning 127: non-pointer type cast to generic pointer
from type 'unsigned-long-int fixed'
  to type 'struct H generic* fixed'
warning 322: Cast of pointer to integer type that cannot represent all values of the pointer type
from type 'unsigned-int generic* fixed'
  to type 'unsigned-int fixed'
*/
#pragma disable_warning 127
#pragma disable_warning 322

struct H
{
  unsigned a;
  unsigned b;
  unsigned c;
};

#if __SIZEOF_POINTER__ >= 4
#define ADDR 0x400000
#else
#define ADDR 0x4000
#endif
#define OFF 0x20

int
main ()
{
  struct H *h = 0;
  unsigned long o;
  volatile int t = 1;

  for (o = OFF; o <= OFF; o += 0x1000)
    {
      struct H *u;
      u = (struct H *) (ADDR + o);
      if (t)
	{
	  h = u;
	  break;
	}
    }

  if (h == 0)
    return 0;
  unsigned *tt = &h->b;
  if ((__SIZE_TYPE__) tt != (ADDR + OFF + __builtin_offsetof (struct H, b)))
    __builtin_abort ();
  return 0;
}
