/* N3886 6.9.2p3: a function with a non-void return type shall have
   a complete return type. */
struct incomplete;

#ifdef TEST1
struct incomplete returns_incomplete(void) /* ERROR */
{
  for (;;)
    ;
}
#endif

#ifdef TEST2
struct incomplete *returns_pointer_to_incomplete(void)
{
  return 0;
}
#endif

#ifdef TEST3
/* The ds390 port does not support returning any aggregate by value. */
#if !defined(__SDCC_ds390)
struct complete { int member; };

struct complete returns_complete(void)
{
  struct complete result = {0};
  return result;
}
#endif
#endif
