/* bug-4042.c

   A function declared at block scope was mistaken for an uninitialized
   automatic object.  These tests also cover related bugs such as
   wrongly reporting "variable 'object' must be static to have storage
   class in reentrant function" for an __xdata object declared explicitly
   extern at block scope.
 */

#ifdef TEST1

void fn(void);

void test_fn(void)
{
  void fn(void);
  fn();
}

void test_extern_fn(void)
{
  extern void fn(void);
  fn();
}

void test_fn_no_prior_declaration(void)
{
  void fn_no_prior_declaration(void);
  fn_no_prior_declaration();
}

void test_extern_fn_no_prior_declaration(void)
{
  extern void extern_fn_no_prior_declaration(void);
  extern_fn_no_prior_declaration();
}

static void fn_prior_internal_linkage(void)
{
}

void test_fn_prior_internal_linkage(void)
{
  void fn_prior_internal_linkage(void);
  fn_prior_internal_linkage();
}

void test_extern_fn_prior_internal_linkage(void)
{
  extern void fn_prior_internal_linkage(void);
  fn_prior_internal_linkage();
}

int obj;

int test_obj(void)
{
  int obj;
  obj = 1;
  return obj;
}

int test_extern_obj(void)
{
  extern int obj;
  return obj;
}

int test_extern_obj_no_prior_declaration(void)
{
  extern int extern_obj_no_prior_declaration;
  return extern_obj_no_prior_declaration;
}

static int obj_prior_internal_linkage;

int test_extern_obj_prior_internal_linkage(void)
{
  extern int obj_prior_internal_linkage;
  return obj_prior_internal_linkage;
}

#if defined(SDCC) && defined(__has_xdata)

void test_xdata(void)
{
  __xdata int xdata_fn(void);
  xdata_fn();
}

void test_xdata_extern_fn(void)
{
  extern __xdata int xdata_fn(void);
  xdata_fn();
}

__xdata int xdata_obj;

int test_xdata_extern_obj(void)
{
  extern __xdata int xdata_obj;
  return xdata_obj;
}

int test_extern_xdata_obj_no_prior_declaration(void)
{
  extern __xdata int extern_xdata_obj_no_prior_declaration;
  return extern_xdata_obj_no_prior_declaration;
}

static __xdata int xdata_obj_prior_internal_linkage;

int test_extern_xdata_obj_prior_internal_linkage(void)
{
  extern __xdata int xdata_obj_prior_internal_linkage;
  return xdata_obj_prior_internal_linkage;
}

#endif

#endif
