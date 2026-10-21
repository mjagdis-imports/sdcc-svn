/* Structure member stores with register pressure. */
#include <testfwk.h>

#ifdef __SDCC
#pragma max_allocs_per_node 512
#endif

#ifndef SDCC_PDK /* The static table is too large for the small PDK targets. */
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
  struct Item items[3];
};

static struct Table table;

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
testRegisterCopyCycle(void)
{
#ifndef SDCC_PDK
  fill (3, &table);
  for (unsigned char i = 0; i < 3; ++i)
    {
      ASSERT (table.items[i].tag == i);
      ASSERT (table.items[i].first == i + 1);
      ASSERT (table.items[i].second == i + 2);
      /* Without the fix, exchanging A and B reverses these two bytes
         on Z80 with IY reserved and this allocation limit. */
      ASSERT (table.items[i].third == i + 3);
      ASSERT (table.items[i].last == i + 4);
    }
#endif
}
