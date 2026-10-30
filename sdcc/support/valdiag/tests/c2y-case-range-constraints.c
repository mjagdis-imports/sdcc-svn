int audit_nonempty;

#ifdef __SDCC
#pragma std_c2y
#elif defined(__GNUC__)
/* GCC implements case ranges as an extension. */
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#ifdef TEST13

/* N3886 6.8.5.3p4: every case range value must survive conversion. */
int range_conversion (unsigned int value)
{
  switch (value)            /* WARNING(SDCC) */
    {
      case 65536 ... 65537:
        return 1;           /* IGNORE */
      default:              /* IGNORE */
        return 0;
    }
}

#endif

#ifdef TEST14

/* A case range representable in the promoted controlling type is valid. */
int valid_range_conversion (unsigned int value)
{
  switch (value)
    {
      case 1 ... 2:
        return 1;
      default:
        return 0;
    }
}

#endif

#ifdef TEST15

/* N3886 6.6.2p3: a case range lower bound is an integer constant expression. */
int nonconstant_lower_bound (int lower, int value)
{
  switch (value)
    {
      case lower ... 3: /* ERROR */
        return 1;
      default:
        return 0;
    }
}

#endif

#ifdef TEST16

/* N3886 6.6.2p3: a case range upper bound is an integer constant expression. */
int nonconstant_upper_bound (int upper, int value)
{
  switch (value)
    {
      case 1 ... upper: /* ERROR */
        return 1;
      default:
        return 0;
    }
}

#endif
