/* Bug #2843: the switch must not produce an unreachable-code warning. */
#ifdef __SDCC
#pragma disable_warning 110
#endif

#ifdef TEST1
int f(int a)
{
  switch (a)
    {
    case 1:
      break;
    case 5:
      return 1;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
      break;
    }
  return 0;
}
#endif
