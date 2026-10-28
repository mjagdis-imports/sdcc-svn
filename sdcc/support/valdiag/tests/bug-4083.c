struct aggregate
{
  int member;
};

/* N3886 6.8.5.2p1: an if controlling expression shall have scalar type. */
#ifdef TEST1
void aggregate_if(void)
{
  struct aggregate value;
  if (value)            /* ERROR */
    ;
}
#endif

/* N3886 6.8.6.1p2: a loop controlling expression shall have scalar type. */
#ifdef TEST2
void aggregate_loop(void)
{
  struct aggregate value;
  while (value)         /* ERROR */
    ;
}
#endif

union union_aggregate
{
  int member;
};

/* N3886 6.8.5.2p1: an if controlling expression shall have scalar type. */
#ifdef TEST3
void union_aggregate_if(void)
{
  union union_aggregate value;
  if (value)            /* ERROR */
    ;
}
#endif

/* N3886 6.8.6.1p2: a loop controlling expression shall have scalar type. */
#ifdef TEST4
void union_aggregate_loop(void)
{
  union union_aggregate value;
  while (value)         /* ERROR */
    ;
}
#endif

/* N3886 6.8.6.1p2: a loop controlling expression shall have scalar type. */
#ifdef TEST5
void aggregate_for(void)
{
  struct aggregate value;
  for (; value; )      /* ERROR */
    ;
}
#endif

/* N3886 6.8.6.1p2: a loop controlling expression shall have scalar type. */
#ifdef TEST6
void union_aggregate_for(void)
{
  union union_aggregate value;
  for (; value; )      /* ERROR */
    ;
}
#endif

/* N3886 6.8.6.1p2: a loop controlling expression shall have scalar type. */
#ifdef TEST7
void aggregate_do(void)
{
  struct aggregate value;
  do
    ;
  while (value);       /* ERROR */
}
#endif

/* N3886 6.8.6.1p2: a loop controlling expression shall have scalar type. */
#ifdef TEST8
void union_aggregate_do(void)
{
  union union_aggregate value;
  do
    ;
  while (value);       /* ERROR */
}
#endif
