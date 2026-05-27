#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// We just need some generator for okayish pseudo-random numbers, but we want the results to be reproducible
// across hosts, so instead of the hosts rand(), we just use a copy of the one from the SDCC library.
// If we ever need outout widert than 15 bits, we can migrate to a wider xorshift variant.

#define SDCC_RAND_MAX 32767

static uint32_t s = 0x80000001;

int16_t sdcc_rand(void)
{
	register uint32_t t = s;

	t ^= t >> 10;
	t ^= t << 9;
	t ^= t >> 25;

	s = t;

	return(t & SDCC_RAND_MAX);
}


int main(int argc, const char *argv[])
{
  if(argc != 2)
    return -1;

  long seqlen = strtol (argv[1], NULL, 0);

  if(seqlen < 0 || seqlen > SDCC_RAND_MAX)
    return -1;

  while(seqlen--)
    printf ("%ld ", (long int)(sdcc_rand()));
  printf("\n");

  return 0;
}

