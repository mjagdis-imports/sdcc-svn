/* Pointer addition removes _Optional from the referenced type while
   preserving the other qualifiers. */

#include <testfwk.h>

static int
function (void)
{
  return 7;
}

typedef int Fn (void);

static Fn *
function_address (_Optional Fn *p)
{
  if (p)
    return *p;
  return 0;
}

#if !defined(__SDCC_pdk13) && !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15)
struct Item
{
  unsigned char tag;
  unsigned char padding[19];
  unsigned short first;
  unsigned short second;
  unsigned short third;
  unsigned char padding2[4];
  unsigned short last;
};

struct Table
{
  struct Item items[8];
};

static __xdata struct Table table;

static void
fill (unsigned short count, void *storage)
{
  struct Table *p = storage;

  for (unsigned char i = 0; i < count; ++i)
    {
      p->items[i].tag = i;
      p->items[i].first = i + 1;
      p->items[i].second = i + 2;
      p->items[i].third = i + 3;
      p->items[i].last = i + 4;
    }
}
#endif

void
testBug (void)
{
  _Optional const volatile int *p = 0;
  int *restrict _Optional *pp = 0;

  ASSERT (function_address (function) () == 7);
  ASSERT (function_address (0) == 0);

  ASSERT (_Generic (p + 0, const volatile int *: 1, default: 0));
  ASSERT (_Generic (0 + p, const volatile int *: 1, default: 0));
  ASSERT (_Generic (pp + 0, int *restrict *: 1, default: 0));
  ASSERT (_Generic (&p[0], const volatile int *: 1, default: 0));

#if !defined(__SDCC_pdk13) && !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15)
  fill (8, &table);
  for (unsigned char i = 0; i < 8; ++i)
    {
      ASSERT (table.items[i].tag == i);
      ASSERT (table.items[i].first == i + 1);
      ASSERT (table.items[i].second == i + 2);
      ASSERT (table.items[i].third == i + 3);
      ASSERT (table.items[i].last == i + 4);
    }
#endif
}
