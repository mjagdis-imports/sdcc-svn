int audit_nonempty;

/* N3886 6.8.2p3: label names shall be unique within a function. */
#ifdef TEST3
void duplicate_label (void)
{
label: /* IGNORE(GCC) */
  ;
label:                  /* ERROR */
  ;
}
#endif

/* The same label name may be used in different functions. */
#ifdef TEST4
void label_in_first_function (void)
{
  goto label;
label: /* IGNORE(GCC) */
  ;
}

void label_in_second_function (void)
{
  goto label;
label: /* IGNORE(GCC) */
  ;
}
#endif

/* N3886 6.8.7.2p1: goto shall name a label in the enclosing function. */
#ifdef TEST5
void missing_label (void)
{
  goto nowhere;         /* ERROR */
}
#endif

/* A goto to a label in the enclosing function is valid. */
#ifdef TEST6
void existing_label (void)
{
  goto destination;
destination:
  ;
}
#endif

/* N3886 6.8.7.3p1: continue shall appear only in a loop body. */
#ifdef TEST7
void continue_outside_loop (void)
{
  continue;             /* ERROR */
}
#endif

/* N3886 6.8.7.4p1: break shall appear only in a switch or loop body. */
#ifdef TEST8
void break_outside_loop (void)
{
  break;                /* ERROR */
}
#endif

/* N3886 6.8.7.5p1: a void function cannot return an integer value. */
#ifdef TEST9
void void_returning_value (void) /* IGNORE(GCC) */
{
  return 0;             /* ERROR(SDCC) */ /* WARNING(GCC) */
}
#endif

/* N3886 6.8.7.5p1: non-void functions shall not use bare return. */
#ifdef TEST10
int value_returning_nothing (void) /* IGNORE(GCC) */
{
  return;               /* WARNING */
}
#endif

/* Matching return statements are valid. */
#ifdef TEST11
void valid_void_return (void)
{
  return;
}

int valid_value_return (void)
{
  return 0;
}
#endif
