/* Facts about const objects whose addresses escape (RFE #1043).
   Dereference warning markers expect _Optional diagnostic 355. The scalar
   tests labelled 110/126 instead use optimisation diagnostics to check that
   valid code is successfully folded and its unreachable return detected. */

#pragma std_c23

#ifdef TEST1
void escape(_Optional int *const *p);
int const_pointer_call(_Optional int *p)
{
  /* Discarding saved's non-null fact at the call would make the final read
     warn. */
  if (!p)
    return 0;
  _Optional int *const saved = p;
  const int before = *saved;
  escape(&saved);
  return before + *saved;
}
#endif

#ifdef TEST2
void escape(_Optional int **p);
int mutable_pointer_call(_Optional int *p)
{
  /* Preserving saved's old value across the call would suppress the required
     warning. */
  if (!p)
    return 0;
  _Optional int *saved = p;
  const int before = *saved;
  escape(&saved); // Could assign saved = nullptr.
  return before + *saved; /* WARNING */
}
#endif

#ifdef TEST3
/* Stack arguments to indirect calls require matching reentrant types. */
#if __has_reentrant || defined(__SDCC_pdk13) || defined(__SDCC_pdk14) || \
    defined(__SDCC_pdk15)
#define REENTRANT __reentrant
#else
#define REENTRANT
#endif
void observer(_Optional int *const *p) REENTRANT;
void (*escape)(_Optional int *const *p) REENTRANT = observer;
int const_pointer_indirect_call(_Optional int *p)
{
  /* Discarding saved's non-null fact at the indirect call would make the
     final read warn. */
  if (!p)
    return 0;
  _Optional int *const saved = p;
  const int before = *saved;
  escape(&saved);
  return before + *saved;
}
#endif

#ifdef TEST4
void escape(_Optional int *const volatile *p);
int const_volatile_pointer_call(_Optional int *p)
{
  /* Treating volatile reads as a stable value would suppress required
     warnings. */
  if (!p)
    return 0;
  _Optional int *const volatile saved = p;
  const int before = *saved; /* WARNING */
  escape(&saved);
  return before + *saved; /* WARNING */
}
#endif

#ifdef TEST5
void escape(const _Optional int **p);
int pointer_to_const_call(_Optional int *p)
{
  /* Mistaking a const pointee for a const pointer would suppress the
     required warning. */
  if (!p)
    return 0;
  const _Optional int *saved = p;
  const int before = *saved;
  escape(&saved); // Could assign saved = nullptr.
  return before + *saved; /* WARNING */
}
#endif

#ifdef TEST6
void escape(_Optional int *const *p);
int const_pointer_store(_Optional int *p, int *other)
{
  /* Discarding saved's non-null fact at the call or store would make the
     final read warn. */
  if (!p)
    return 0;
  _Optional int *const saved = p;
  const int before = *saved;
  escape(&saved);
  *other = 0;
  return before + *saved;
}
#endif

#ifdef TEST7
void escape_const_view(_Optional int *const *p);
void mutable_object_const_view(_Optional int *p)
{
  /* Trusting the const access path would suppress the warning for the
     mutable pointer. */
  if (!p)
    return;
  _Optional int *saved = p;
  escape_const_view(&saved); // Could clear saved through a mutable alias.
  *saved = 2; /* WARNING */
}
#endif

#ifdef TEST8
void escape(_Optional int *const *p);
void mutable_pointee(_Optional int **p)
{
  /* Treating a const pointer's pointee as immutable would suppress the
     required warning. */
  if (!*p)
    return;
  _Optional int **const saved = p;
  escape(saved); // Could assign *p = nullptr.
  **saved = 2; /* WARNING */
}
#endif

#ifdef TEST9
void escape(const char *p);
int const_scalar_call(void)
{
  /* The call cannot change value == 9. Folding the final if must emit 110
     (conditional flow changed) and 126 (unreachable return). Losing the
     value fact would suppress both optimisation diagnostics. */
  const char value = 9;
  escape(&value);
  if (value != 9) /* WARNING */ /* 110: conditional flow changed */
    return 0; /* WARNING */ /* 126: unreachable code */
  return 1;
}
#endif

#ifdef TEST10
void escape(const char *p);
void mutable_scalar_call(int *p)
{
  /* Reusing value == 9 after the call would suppress the required
     dereference warning. */
  char value = 9;
  escape(&value); // Could assign value = 0.
  _Optional int *saved = value == 9 ? p : nullptr;
  *saved = 1; /* WARNING */
}
#endif

#ifdef TEST11
void escape(_Optional int *const *p, _Optional int **q);
void original_pointer_changes(_Optional int *p)
{
  /* Losing saved's fact would add a warning; retaining p's fact would remove
     one. */
  if (!p)
    return;
  _Optional int *const saved = p;
  escape(&saved, &p); // Could assign p = nullptr, but not change saved.
  *saved = 1;
  *p = 2; /* WARNING */
}
#endif

#ifdef TEST12
void escape(const volatile char *p);
void const_volatile_scalar_call(int *p)
{
  /* Reusing the volatile initialiser would suppress the required dereference
     warning. */
  const volatile char value = 9;
  escape(&value);
  _Optional int *saved = value == 9 ? p : nullptr;
  *saved = 1; /* WARNING */
}
#endif

#ifdef TEST13
int target;
_Optional int *const global_saved = &target;
void escape(void);
void const_global_pointer_call(void)
{
  /* Losing the global's non-null fact at the call would make the final store
     warn. */
  if (!global_saved)
    return;
  escape();
  *global_saved = 1;
}
#endif

#ifdef TEST14
static int target;
static _Optional int *const global_saved = &target;
void escape(_Optional int *const *p);
void static_const_global_pointer_call(void)
{
  /* Losing the static global's non-null fact at the call would make the
     final store warn. */
  if (!global_saved)
    return;
  escape(&global_saved);
  *global_saved = 1;
}
#endif

#ifdef TEST15
int target;
_Optional int *global_saved = &target;
void escape(_Optional int *const *p);
void mutable_global_pointer_const_view(void)
{
  /* Trusting the const access path would suppress the warning for the
     mutable global. */
  if (!global_saved)
    return;
  escape(&global_saved); // Could assign global_saved = nullptr.
  *global_saved = 1; /* WARNING */
}
#endif

#ifdef TEST16
int target;
_Optional int *const volatile global_saved = &target;
void escape(void);
void const_volatile_global_pointer_call(void)
{
  /* Reusing the checked volatile pointer value would suppress the required
     warning. */
  if (!global_saved)
    return;
  escape();
  *global_saved = 1; /* WARNING */
}
#endif

#ifdef TEST17
const char global_value = 0;
void escape(void);
int const_global_scalar_call(void)
{
  /* The global remains zero across the call. The if must emit 110
     (conditional flow changed), and its return 126 (unreachable code).
     Losing the zero value would suppress both optimisation diagnostics. */
  escape();
  if (global_value) /* WARNING */ /* 110: conditional flow changed */
    return 0; /* WARNING */ /* 126: unreachable code */
  return 1;
}
#endif

#ifdef TEST18
static const char global_value = 9;
void escape(const char *p);
int static_const_global_scalar_call(void)
{
  /* The global remains 9 across the call. The if must emit 110 (conditional
     flow changed), and its return 126 (unreachable code). Losing the value
     fact would suppress both optimisation diagnostics. */
  escape(&global_value);
  if (global_value != 9) /* WARNING */ /* 110: conditional flow changed */
    return 0; /* WARNING */ /* 126: unreachable code */
  return 1;
}
#endif

#ifdef TEST19
int target;
_Optional int *const global_saved = &target;
void const_global_pointer_store(int *other)
{
  /* Discarding the global's non-null fact at the indirect write would make
     the final store warn. */
  if (!global_saved)
    return;
  *other = 0;
  *global_saved = 1;
}
#endif

#ifdef TEST20
int target;
_Optional int *global_pointer = &target;
_Optional int **const global_saved = &global_pointer;
void escape(void);
void const_global_pointer_mutable_pointee(void)
{
  /* Treating the global pointer's pointee as immutable would suppress the
     required warning. */
  if (!*global_saved)
    return;
  escape(); // Could assign *global_saved = nullptr.
  **global_saved = 1; /* WARNING */
}
#endif

#ifdef TEST21
const volatile int global_value = 9;
void escape(void);
void const_volatile_global_scalar_call(int *p)
{
  /* Reusing the checked volatile value would suppress the required
     dereference warning. */
  if (global_value != 9)
    return;
  escape();
  _Optional int *saved = global_value == 9 ? p : nullptr;
  *saved = 1; /* WARNING */
}
#endif

#ifdef TEST22
int global_value = 9;
void escape(void);
void mutable_global_scalar_call(int *p)
{
  /* Reusing the checked global value after the call would suppress the
     required warning. */
  if (global_value != 9)
    return;
  escape(); // Could assign global_value = 0.
  _Optional int *saved = global_value == 9 ? p : nullptr;
  *saved = 1; /* WARNING */
}
#endif

#ifdef TEST23
void const_lvalue_alias_store(_Optional int *const *view,
                             _Optional int **alias)
{
  /* Assuming *view is immutable would suppress the required warning. */
  if (!*view)
    return;
  *alias = nullptr; // *alias could be *view.
  **view = 1; /* WARNING */
}
#endif

#ifdef TEST24
void escape(_Optional int *const *p);
void const_lvalue_call(_Optional int *const *view)
{
  /* Treating *view as immutable across the call would suppress the warning. */
  if (!*view)
    return;
  escape(view); // Could assign nullptr to *view through a mutable alias.
  **view = 1; /* WARNING */
}
#endif

#ifdef TEST25
struct pointer_holder { _Optional int *p; };
void const_member_alias_store(const struct pointer_holder *view,
                             struct pointer_holder *alias)
{
  /* Assuming view->p is immutable would suppress the required warning. */
  if (!view->p)
    return;
  alias->p = nullptr; // alias->p could be view->p.
  *view->p = 1; /* WARNING */
}
#endif

#ifdef TEST26
struct pointer_holder { _Optional int *p; };
void escape(const struct pointer_holder *p);
void const_member_call(const struct pointer_holder *view)
{
  /* Trusting the const member access would suppress the required warning. */
  if (!view->p)
    return;
  escape(view); // Could clear view->p through a mutable alias.
  *view->p = 1; /* WARNING */
}
#endif

#ifdef TEST27
void const_element_alias_store(_Optional int *const *view,
                              _Optional int **alias)
{
  /* Assuming view[1] is immutable would suppress the required warning. */
  if (!view[1])
    return;
  alias[1] = nullptr; // alias[1] could be view[1].
  *view[1] = 1; /* WARNING */
}
#endif
