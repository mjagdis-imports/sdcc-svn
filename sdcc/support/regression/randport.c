#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// Should match randseq.c
#define SDCC_RAND_MAX 32767

#define NUM_BASEPORTS 4
const char *baseports[NUM_BASEPORTS] = {"ds390", "f8", "f8l", "hc08"};

#define NUM_OPTIONS 7
const char *options[NUM_OPTIONS] = {"--opt-code-speed", "--opt-code-size", "--nopurity", "--nolospre", "--nogenconstprop", "--nolabelopt", "--noloopreverse"};

#define BUFFERLEN 256

int main(int argc, const char *argv[])
{
  char buffer[BUFFERLEN];

  if (argc != 2)
    return -1;

  long seed = strtol (argv[1], NULL, 0);
  unsigned long randbits = seed;

  if(seed < 0 || seed > 32768)
    return -1;

  int baseportbits = ceil(log2(NUM_BASEPORTS));

  const char *baseport = baseports[randbits % NUM_BASEPORTS];
  randbits >>= baseportbits;

  printf("%s\n", baseport);

  snprintf(buffer, BUFFERLEN - 1, "cp -r ports/%s ports/random-%ld\n", baseport, seed);
  system(buffer);

  for (int i = 0; i < NUM_OPTIONS; i++)
    {
      if(randbits & 1)
        {
          snprintf(buffer, BUFFERLEN - 1, "sed -i '1s;^;SDCCFLAGS += %s\\n;' ports/random-%ld/spec.mk\n", options[i], seed);
          system(buffer);
        }
      randbits >>= 1;
    }

  // Todo: handle --max-allocs-per-node

  snprintf(buffer, BUFFERLEN - 1, "sed -i '1s;^;# Randomized regression testing for seed %ld\\n;' ports/random-%ld/spec.mk\n", seed, seed);
  system(buffer);

  return 0;
}

