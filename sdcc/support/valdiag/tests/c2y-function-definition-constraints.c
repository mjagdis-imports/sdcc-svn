int audit_nonempty;

/* N3886 6.9.1p4: an internal-linkage identifier has at most one
   external definition in a translation unit. */
#ifdef TEST8
static int duplicate_definition (void) /* IGNORE(GCC) */
{
  return 0;
}

static int duplicate_definition (void) /* ERROR */
{
  return 1;
}
#endif

/* N3886 6.9.2p2: the identifier defined as a function has function type. */
#ifdef TEST9
int object                         /* ERROR(SDCC) */
{ /* ERROR(GCC) */
}
#endif

/* N3886 6.9.2p4: a storage-class specifier on a function definition is
   extern or static. */
#ifdef TEST10
auto int auto_function_definition(void) /* ERROR(SDCC) */ /* WARNING(GCC) */
{
  return 0;
}
#endif

/* A used internal-linkage function with exactly one definition is valid. */
#ifdef TEST11
static int defined_once (void)
{
  return 1;
}

int use_defined_once (void)
{
  return defined_once();
}
#endif

/* An ordinary function definition defines an identifier of function type. */
#ifdef TEST12
int ordinary_function (void)
{
  return 0;
}
#endif

/* extern and static are valid on function definitions. */
#ifdef TEST14
static int static_definition (void)
{
  return 0;
}

extern int extern_definition (void)
{
  return static_definition();
}
#endif
