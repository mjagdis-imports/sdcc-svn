/* suppress warning W_SIZEOF_VOID, &q-&q == 0 => 0*sizeof(void) == 0 */
#pragma disable_warning 178

void abort (void);

char q;
void *a[2];

void foo (char *p)
{
  int i;
  for (i = 0; i < 2; i++)
    a[i] += p - &q;
}

int main (void)
{
  int i;
  foo (&q);
  for (i = 0; i < 2; i ++)
    if (a[i])
      abort ();
  return 0;
}
