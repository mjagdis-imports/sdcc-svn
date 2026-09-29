/* Regression test for bug #4073: calling through a pointer to an
   optional-qualified function is an implicit dereference. */

#pragma std_c23

typedef void Function (void);

#ifdef TEST1
void implicit_dereference (_Optional Function *pointer)
{
  pointer (); /* WARNING */
}
#endif

#ifdef TEST2
void explicit_dereference (_Optional Function *pointer)
{
  (*pointer) (); /* WARNING */
}
#endif

#ifdef TEST3
void guarded_dereferences (_Optional Function *pointer)
{
  if (pointer)
    {
      pointer ();
      (*pointer) ();
    }
}
#endif
