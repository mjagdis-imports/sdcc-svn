#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

// When adding a port here, also adjust EXCLUDE_extra in Makefile.in!
#define NUM_BASEPORTS 37
const char *baseports[NUM_BASEPORTS] = {
  "ds390", "f8", "f8l", "hc08", "mcs51-huge", "mcs51-large", "mcs51-large-stack-auto", "mcs51-medium",
  "mcs51-small", "mcs51-small-stack-auto", "pdk14", "pdk15", "pdk15-stack-auto", "s08", "s08-stack-auto", "stm8",
  "stm8-large", "tlcs90", "uc6502", "uc6502-stack-auto", "uc65c02", "ucez80", "ucgbz80", "ucr2k",
  "ucr2ka", "ucr3ka", "ucr4k", "ucr5k", "ucr6k", "ucr800", "ucz180", "ucz180-resiy",
  "ucz80", "ucz80n", "ucz80-resiy", "ucz80-undoc", "ucz80-unsafe-read"};

// Switching off optimizations for these ports often results in the binary no longer fitting into memory, and that becomes the main cause of test failures.
const bool lowmemport[NUM_BASEPORTS] = {
  false, false, false, false, false, false, false, false,
  false, false, true,  true,  true,  false, false, false,
  false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false,
  false, false, false, false, false};

#define NUM_OPTIONS 6
const char *options[NUM_OPTIONS] = {"--nopurity", "--nolospre", "--nogenconstprop", "--nolabelopt", "--noloopreverse", "--no-peep"};

#define BUFFERLEN 256

void append_option(const char *option, unsigned long long int seed)
{
  char buffer[BUFFERLEN] = {0};
  printf ("%s ", option);
  snprintf (buffer, BUFFERLEN - 1, "sed -i '1s;^;SDCCFLAGS += %s\\n;' ports/random-%llu/spec.mk\n", option, seed);
  system (buffer);
}

int main (int argc, const char *argv[])
{
  char buffer[BUFFERLEN] = {0};

  if (argc != 2)
    {
      fprintf (stderr, "Usage: randport <seed>\n");
      return -1;
    }

  errno = 0;
  unsigned long long int seed = strtoul (argv[1], NULL, 0);
  unsigned long long int randbits = seed;
  int randbits_remaining = 64;

  if (errno)
    return -1;

  int baseportbits = ceil (log2 (NUM_BASEPORTS));

  const char *baseport = baseports[randbits % NUM_BASEPORTS];
  bool lowmem = lowmemport[randbits % NUM_BASEPORTS];
  randbits >>= baseportbits;
  randbits_remaining -= baseportbits;

  printf ("Creating random-%llu based on %s with additional options: ", seed, baseport);
  snprintf (buffer, BUFFERLEN - 1, "cp -r ports/%s ports/random-%llu\n", baseport, seed);
  system (buffer);

  switch (randbits % 3)
    {
    case 1:
      append_option ("--opt-code-size", seed);
      break;
    case 2:
      append_option ("--opt-code-speed", seed);
      break;
    }
  randbits >>= 2;
  randbits_remaining -= 2;

  if (!lowmem)
    for (int i = 0; i < NUM_OPTIONS; i++)
      {
        if (randbits & 1)
          append_option (options[i], seed);
        randbits >>= 1;
        randbits_remaining--;
      }

  if (randbits_remaining < 4)
    fprintf (stderr, "Error: insufficient pseudorandom bits remaining!\n");

  unsigned long int maxallocs = 128 << (randbits % 10);
  snprintf (buffer, BUFFERLEN - 1, "--max-allocs-per-node %lu", maxallocs);
  append_option (buffer, seed);

  printf ("\n");
  snprintf (buffer, BUFFERLEN - 1, "sed -i '1s;^;# Randomized regression testing for seed %llu\\n;' ports/random-%llu/spec.mk\n", seed, seed);
  system (buffer);

  return 0;
}

