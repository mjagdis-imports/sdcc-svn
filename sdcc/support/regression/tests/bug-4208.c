/* Infer array lengths after replacing generic selections. */
#include <testfwk.h>

#ifdef __SDCC
#pragma std_c2y
#define TEST_GENERIC_STRING
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ > 202311L
#define TEST_GENERIC_STRING
#endif

#ifdef TEST_GENERIC_STRING
static char selected[] = _Generic (1, int: "abc", default: "defgh");
static char fallback[] = _Generic (1L, int: "a", default: "defgh");
static char braced[] = {_Generic (1, int: "xyz", default: "a")};
static char nested[] = _Generic (1, int: _Generic (1L, long: "hi", default: "a"), default: "b");
static char empty[] = _Generic (1, int: "", default: "a");
#endif

void
testGenericStringInitialisers (void)
{
#ifdef TEST_GENERIC_STRING
  char local[] = _Generic (1, int: "abc", default: "a");
  char explicit_length[4] = _Generic (1, int: "abc", default: "a");
  char parenthesised[] = ("abc");

  /* Check the selected and default associations' different lengths. */
  ASSERT (sizeof selected == 4);
  if (sizeof selected == 4)
    {
      ASSERT (selected[0] == 'a');
      ASSERT (selected[1] == 'b');
      ASSERT (selected[2] == 'c');
      ASSERT (selected[3] == 0);
    }
  ASSERT (sizeof fallback == 6);
  if (sizeof fallback == 6)
    {
      ASSERT (fallback[0] == 'd');
      ASSERT (fallback[4] == 'h');
      ASSERT (fallback[5] == 0);
    }

  /* Braces, nested selections and the empty string also infer lengths. */
  ASSERT (sizeof braced == 4);
  if (sizeof braced == 4)
    {
      ASSERT (braced[2] == 'z');
      ASSERT (braced[3] == 0);
    }
  ASSERT (sizeof nested == 3);
  if (sizeof nested == 3)
    {
      ASSERT (nested[1] == 'i');
      ASSERT (nested[2] == 0);
    }
  ASSERT (sizeof empty == 1);
  ASSERT (empty[0] == 0);

  /* Local initialisation must copy every character and the terminator. */
  ASSERT (sizeof local == 4);
  if (sizeof local == 4)
    {
      ASSERT (local[1] == 'b');
      ASSERT (local[2] == 'c');
      ASSERT (local[3] == 0);
    }

  /* These forms were already correct before the fix. */
  ASSERT (sizeof explicit_length == 4);
  ASSERT (explicit_length[2] == 'c');
  ASSERT (explicit_length[3] == 0);
  ASSERT (sizeof parenthesised == 4);
  ASSERT (parenthesised[2] == 'c');
  ASSERT (parenthesised[3] == 0);
#endif
}
