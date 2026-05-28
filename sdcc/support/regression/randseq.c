#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

// We just need some generator for okayish pseudo-random numbers, but we want the results to be reproducible
// across hosts, so instead of the hosts rand(), we just our own here. Can't use the one from the SDCC library,
// since we want wider output here.

uint64_t rol64 (uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

uint64_t xoshiro256ss (void) {
  static uint64_t s[4] = {0, 0, 1, 2};
  const uint64_t result = rol64(s[1] * 5, 7) * 9;
  const uint64_t t = s[1] << 17;

  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];

  s[2] ^= t;
  s[3] = rol64(s[3], 45);

  return result;
}

int main (int argc, const char *argv[])
{
  if(argc != 2)
    {
      fprintf (stderr, "Usage: randseq <sequence length>\n");
      return -1;
    }

  errno = 0;
  unsigned long seqlen = strtoul (argv[1], NULL, 0);
  if (errno)
    return -1;

  while (seqlen--)
    printf ("%llu ", (unsigned long long)(xoshiro256ss()));
  printf ("\n");

  return 0;
}

