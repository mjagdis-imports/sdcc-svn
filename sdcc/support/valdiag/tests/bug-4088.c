/* N3886 6.5.5p4: a pointer may not be cast to floating type. */
#ifdef TEST1
int *pointer;
float test(void)
{
  return (float)pointer; /* ERROR */
}
#endif
