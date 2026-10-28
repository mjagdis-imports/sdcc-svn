/* Exercise warning controls. */
typedef int enable_warning_translation_unit;

#ifdef __SDCC

#ifdef TEST1
/* Existing warnings remain enabled by default. */
void test (char *p)
{
  int *q = p; /* WARNING(SDCC) */
  (void)q;
}
#endif

#ifdef TEST2
/* Disabling a warning still suppresses it. */
#pragma disable_warning 244
void test (char *p)
{
  int *q = p;
  (void)q;
}
#endif

#ifdef TEST3
/* A disabled warning can be re-enabled. */
#pragma disable_warning 244
#pragma enable_warning 244
void test (char *p)
{
  int *q = p; /* WARNING(SDCC) */
  (void)q;
}
#endif

#ifdef TEST4
/* The last pragma for a warning determines its state. */
#pragma enable_warning 244
#pragma disable_warning 244
void test (char *p)
{
  int *q = p;
  (void)q;
}
#endif

#endif
