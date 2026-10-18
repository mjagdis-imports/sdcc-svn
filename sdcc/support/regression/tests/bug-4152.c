/* Stack shifts must preserve a live pointer in X and the shift flags. */
#include <testfwk.h>
#include <stddef.h>

struct Ring
{
  size_t size;
};

static struct Ring ring_storage;
static size_t observed;

void reset (struct Ring *ring);

void set_left (struct Ring *ring, unsigned int shift)
{
  ring->size = (size_t)(1ul << shift);
  reset (ring);
}

void set_right (struct Ring *ring, unsigned int shift)
{
  ring->size = (size_t)(0x8000ul >> shift);
  reset (ring);
}

void reset (struct Ring *ring)
{
  ASSERT (ring == &ring_storage);
  observed = ring->size;
}

void testBug (void)
{
  static const size_t powers[] = {
    1, 2, 4, 8, 16, 32, 64, 128,
    256, 512, 1024, 2048, 4096, 8192, 16384, 32768
  };
  unsigned char shift;

  for (shift = 0; shift < 16; shift++)
    {
      /* Include zero shifts and carry between the low and high bytes. */
      set_left (&ring_storage, shift);
      ASSERT (ring_storage.size == powers[shift]);
      ASSERT (observed == powers[shift]);

      /* The shared stack-access helper is also used by right shifts. */
      set_right (&ring_storage, shift);
      ASSERT (ring_storage.size == powers[15 - shift]);
      ASSERT (observed == powers[15 - shift]);
    }
}
