/* bug-4004.c

   Missing diagnostics for discarded or incompatible pointer target qualifiers
   in function argument conversions.
 */

#ifdef TEST1
/* Implicit argument conversions must diagnose discarded immediate target qualifiers. */

void take_void(void *);
void take_char(char *);
void take_pointer(int **);

void test(void)
{
  volatile char *pv = 0;
  take_void(pv); // pointer target lost volatile qualifier /* WARNING */
  take_char(pv); // pointer target lost volatile qualifier /* WARNING */

  const char *pc = 0;
  take_void(pc); // pointer target lost const qualifier /* WARNING */
  take_char(pc); // pointer target lost const qualifier /* WARNING */

  _Optional char *poc = 0;
  take_void(poc); // pointer target lost _Optional qualifier /* WARNING */
  take_char(poc); // pointer target lost _Optional qualifier /* WARNING */

  int *restrict *pr = 0;
  take_pointer(pr); // pointer target lost restrict qualifier /* WARNING */
  take_void(pr); // pointer target lost restrict qualifier /* WARNING */
}

#endif

#ifdef TEST2
/* Matching target qualifiers and permitted qualification additions must be accepted,
   including combinations of supported qualifiers and conversions to void pointers. */

void set_space(void);
__addressmod set_space space;

void take_const(const int *);
void take_volatile(volatile int *);
void take_optional(_Optional int *);
void take_restrict(int *restrict *);
void take_const_volatile(const volatile int *);
void take_optional_const(_Optional const int *);
void take_optional_volatile(_Optional volatile int *);
void take_optional_const_volatile(_Optional const volatile int *);

void take_const_void(const void *);
void take_volatile_void(volatile void *);
void take_optional_void(_Optional void *);

void take_space(space int *);

void test_preserved_qualifiers(int *p,
          const int *pc,
          volatile int *pv,
          _Optional int *poi,
          const volatile int *pcv,
          _Optional volatile int *pov,
          _Optional const int *poc,
          _Optional const volatile int *pocv,
          int **pp,
          int *restrict *pr,
          space int *ps)
{
  take_const(p);
  take_const(pc);

  take_volatile(p);
  take_volatile(pv);

  take_optional(p);
  take_optional(poi);

  take_restrict(pp);
  take_restrict(pr);

  take_const_volatile(p);
  take_const_volatile(pc);
  take_const_volatile(pv);
  take_const_volatile(pcv);

  take_optional_const(p);
  take_optional_const(pc);
  take_optional_const(poi);
  take_optional_const(poc);

  take_optional_volatile(p);
  take_optional_volatile(pv);
  take_optional_volatile(poi);
  take_optional_volatile(pov);

  take_optional_const_volatile(p);
  take_optional_const_volatile(pc);
  take_optional_const_volatile(pv);
  take_optional_const_volatile(poi);
  take_optional_const_volatile(pcv);
  take_optional_const_volatile(pov);
  take_optional_const_volatile(poc);
  take_optional_const_volatile(pocv);

  take_const_void(p);
  take_const_void(pc);

  take_volatile_void(p);
  take_volatile_void(pv);

  take_optional_void(p);
  take_optional_void(poi);

  take_space(ps);
}

#endif

#ifdef TEST3
/* Optional function targets follow the same argument conversion rules as object targets. */

typedef int CallbackFunc(void);

void take_callback(CallbackFunc *);
void take_optional_callback(_Optional CallbackFunc *);

void test_function_pointer_conversions(_Optional CallbackFunc *po, CallbackFunc *p)
{
  take_callback(po); // pointer target lost _Optional qualifier /* WARNING */
  take_optional_callback(p);
  take_optional_callback(po);
}

#endif

#ifdef TEST4
/* Array argument conversion must preserve qualifiers that the destination would discard. */

void take_int(int *);
void take_void(void *);
void take_int_pointer(int **);
void take_void_pointer(void **);

void test_array_conversions(void)
{
  const int const_array[1] = {0};
  volatile int volatile_array[1] = {0};
  int *restrict restrict_array[1] = {0};
  _Optional int *optional_array[1] = {0};

  take_int(const_array); // pointer target lost const qualifier /* WARNING */
  take_int(volatile_array); // pointer target lost volatile qualifier /* WARNING */
  take_int_pointer(restrict_array); // pointer target lost restrict qualifier /* WARNING */

  take_void(const_array); // pointer target lost const qualifier /* WARNING */
  take_void(volatile_array); // pointer target lost volatile qualifier /* WARNING */
  take_void_pointer(restrict_array); // pointer target lost restrict qualifier /* WARNING */
  take_int_pointer(optional_array); /* WARNING */
  take_void_pointer(optional_array); /* WARNING */
}

#endif

#ifdef TEST5
/* Union member arguments must preserve declared and inherited volatile qualification. */

union U
{
  char bytes[4];
  volatile char volatile_bytes[4];
  char *pointer;
  volatile char *pv;
};

void take_char(char *);

void test_union_members(union U *u, volatile union U *vu)
{
  take_char(u->bytes);
  take_char(vu->bytes); // pointer target lost volatile qualifier /* WARNING */
  take_char(u->volatile_bytes); // pointer target lost volatile qualifier /* WARNING */
  take_char(u->pointer);
  take_char(u->pv); // pointer target lost volatile qualifier /* WARNING */
}

#endif

#ifdef TEST6
/* Pointer-to-pointer arguments must diagnose immediate qualifier loss and deeper mismatches. */
void set_space(void);
__addressmod set_space space;

void take_void_pointer(void **);
void take_char_pointer(char **);
void take_int_pointer(int **);
void take_const_char_pointer(const char **);

void test(void)
{
  const char *pc = 0;
  volatile char *pv = 0;
  char *p = 0;
  int *restrict pr = 0;
  _Optional char *poc = 0;
  space char *ps = 0;
  take_void_pointer(&pc); /* WARNING */
  take_char_pointer(&pc); /* WARNING */
  take_int_pointer(&pc); /* WARNING */
  take_void_pointer(&pv); /* WARNING */
  take_char_pointer(&pv); /* WARNING */
  take_int_pointer(&pv); /* WARNING */
  take_const_char_pointer(&p); /* WARNING */
  take_int_pointer(&pr); // pointer target lost restrict qualifier /* WARNING */
  take_void_pointer(&poc); /* WARNING */
  take_char_pointer(&poc); /* WARNING */
  take_int_pointer(&poc); /* WARNING */
  take_void_pointer(&ps); /* WARNING */
  take_char_pointer(&ps); /* WARNING */
}
#endif

#ifdef TEST7
/* Adding or removing qualifiers on an intervening pointer is incompatible; matching types
   provide valid controls. */
void take_plain(int ***);
void take_const(int *const **);
void take_volatile(int *volatile **);
void take_restrict(int *restrict **);
void take_optional(int *_Optional **);

void test(int ***p, int *const **pc, int *volatile **pv,
          int *restrict **pr, int *_Optional **po)
{
  take_plain(pc); /* WARNING */
  take_plain(pv); /* WARNING */
  take_plain(pr); /* WARNING */
  take_plain(po); /* WARNING */
  take_const(p); /* WARNING */
  take_volatile(p); /* WARNING */
  take_restrict(p); /* WARNING */
  take_optional(p); /* WARNING */
  take_plain(p);
  take_const(pc);
  take_volatile(pv);
  take_restrict(pr);
  take_optional(po);
}
#endif

#ifdef TEST8
/* Immediate qualification additions are valid; the pointer-to-array negative control
   checks that incompatible array element types are still diagnosed. */
void take_const_pointer(char *const *);
void take_restrict_pointer(char *restrict *);
void take_volatile_pointer(char *volatile *);
void take_optional_pointer(char *_Optional *);
/* Keep array parameter names to avoid nested abstract-declarator bug #4105. */
void take_const_array(const int (*p)[2]);
void take_const_target(const char **);
void take_void(void *);
void take_array(int *(*p)[2]);

void test(char **p, const char **pc, int (*a)[2], const int *(*ca)[2])
{
  take_const_pointer(p);
  take_restrict_pointer(p);
  take_volatile_pointer(p);
  take_optional_pointer(p);
  take_const_array(a);
  take_const_target(pc);
  take_void(pc);
  /* ca points to an array of pointers to const int, while take_array expects
     an array of pointers to int. These array element types are incompatible. */
  take_array(ca); /* WARNING */
}
#endif

/* Controls and conversions that remain invalid under N3449. */

#if TEST9 || TEST17
/* Shared function types for top-level parameter qualifier and contravariance checks. */
#if __has_reentrant
#define REENTRANT __reentrant
#else
#define REENTRANT
#endif
typedef void PlainParamFunc(int, char *) REENTRANT;
typedef void ConstPointeeParamFunc(int, const char *) REENTRANT;
typedef void VolatilePointeeParamFunc(int, volatile char *) REENTRANT;
typedef void OptionalPointeeParamFunc(int, _Optional char *) REENTRANT;
typedef void PointerPointeeParamFunc(int, char **) REENTRANT;
typedef void RestrictPointeeParamFunc(int, char *restrict *) REENTRANT;
typedef void ConstPointerParamFunc(int, char *const) REENTRANT;
typedef void VolatilePointerParamFunc(int, char *volatile) REENTRANT;
typedef void RestrictPointerParamFunc(int, char *restrict) REENTRANT;
PlainParamFunc plain_param;
ConstPointeeParamFunc const_pointee_param;
VolatilePointeeParamFunc volatile_pointee_param;
OptionalPointeeParamFunc optional_pointee_param;
PointerPointeeParamFunc pointer_pointee_param;
RestrictPointeeParamFunc restrict_pointee_param;
void take_plain_param(PlainParamFunc *);
void take_const_pointer_param(ConstPointerParamFunc *);
void take_volatile_pointer_param(VolatilePointerParamFunc *);
void take_restrict_pointer_param(RestrictPointerParamFunc *);
void take_const_pointee_param(ConstPointeeParamFunc *);
void take_volatile_pointee_param(VolatilePointeeParamFunc *);
void take_optional_pointee_param(OptionalPointeeParamFunc *);
void take_pointer_pointee_param(PointerPointeeParamFunc *);
void take_restrict_pointee_param(RestrictPointeeParamFunc *);

#endif

#ifdef TEST9
/* Top-level parameter CVR qualifiers are ignored, but parameter-target qualifiers matter.
   Separate functions distinguish the ignored qualifiers from incompatible conversions. */
void test(PlainParamFunc *p, ConstPointerParamFunc *pc,
          VolatilePointerParamFunc *pv, RestrictPointerParamFunc *pr)
{
  /* Top-level CVR parameter qualifiers are ignored, in either direction. */
  take_plain_param(pc);
  take_plain_param(pv);
  take_plain_param(pr);
  take_plain_param(plain_param);
  take_const_pointer_param(p);
  take_const_pointer_param(pc);
  take_volatile_pointer_param(p);
  take_volatile_pointer_param(pv);
  take_restrict_pointer_param(p);
  take_restrict_pointer_param(pr);
}

/* Supply a function taking an unqualified target where the expected function
   takes a qualified target: the opposite of parameter contravariance.
   Calling through the expected type could discard target qualifiers. */
void test_reverse(PlainParamFunc *p, PointerPointeeParamFunc *pp)
{
  take_const_pointee_param(p); /* WARNING */
  take_volatile_pointee_param(p); /* WARNING */
  take_optional_pointee_param(p); /* WARNING */
  take_restrict_pointee_param(pp); /* WARNING */
}
#endif

#if TEST10 || TEST18
/* Shared function types for ignored return qualifiers and return covariance. */
/* Suppress advice about return qualifiers; test compatibility diagnostics. */
#pragma disable_warning 361
#if __has_reentrant
#define REENTRANT __reentrant
#else
#define REENTRANT
#endif
typedef int ScalarReturnFunc(void) REENTRANT;
typedef const int ConstScalarReturnFunc(void) REENTRANT;
typedef volatile int VolatileScalarReturnFunc(void) REENTRANT;
typedef char *PointerReturnFunc(void) REENTRANT;
typedef char *const ConstPointerReturnFunc(void) REENTRANT;
typedef char *volatile VolatilePointerReturnFunc(void) REENTRANT;
typedef char *restrict RestrictPointerReturnFunc(void) REENTRANT;
typedef char *_Optional OptionalPointerReturnFunc(void) REENTRANT;
typedef const char *ConstPointeeReturnFunc(void) REENTRANT;
typedef volatile char *VolatilePointeeReturnFunc(void) REENTRANT;
typedef _Optional char *OptionalPointeeReturnFunc(void) REENTRANT;
/* Work around SDCC bug #4104: direct char ** function-return typedefs
   with __reentrant are mishandled. */
typedef char *CharPointer;
typedef CharPointer *PointerPointerReturnFunc(void) REENTRANT;
typedef CharPointer restrict *RestrictPointeeReturnFunc(void) REENTRANT;
void take_scalar_return(ScalarReturnFunc *);
void take_const_scalar_return(ConstScalarReturnFunc *);
void take_volatile_scalar_return(VolatileScalarReturnFunc *);
void take_pointer_return(PointerReturnFunc *);
void take_const_pointer_return(ConstPointerReturnFunc *);
void take_volatile_pointer_return(VolatilePointerReturnFunc *);
void take_restrict_pointer_return(RestrictPointerReturnFunc *);
void take_optional_pointer_return(OptionalPointerReturnFunc *);
void take_const_pointee_return(ConstPointeeReturnFunc *);
void take_volatile_pointee_return(VolatilePointeeReturnFunc *);
void take_optional_pointee_return(OptionalPointeeReturnFunc *);
void take_pointer_pointer_return(PointerPointerReturnFunc *);
void take_restrict_pointee_return(RestrictPointeeReturnFunc *);

#endif

#ifdef TEST10
/* Top-level return qualifiers are ignored, but referenced qualifiers matter. Separate
   functions distinguish ignored qualifiers, matching referenced types and qualifier loss. */
void test(ScalarReturnFunc *sfp,
          ConstScalarReturnFunc *pcs,
          VolatileScalarReturnFunc *pvs,
          PointerReturnFunc *p,
          ConstPointerReturnFunc *pc,
          VolatilePointerReturnFunc *pv,
          RestrictPointerReturnFunc *pr,
          OptionalPointerReturnFunc *po)
{
  /* Top-level return qualifiers are ignored, in either direction. */
  take_scalar_return(pcs);
  take_scalar_return(sfp);
  take_scalar_return(pvs);
  take_const_scalar_return(sfp);
  take_const_scalar_return(pcs);
  take_volatile_scalar_return(sfp);
  take_volatile_scalar_return(pvs);
  take_pointer_return(pc);
  take_pointer_return(pv);
  take_pointer_return(pr);
  take_pointer_return(po);
  take_pointer_return(p);
  take_const_pointer_return(p);
  take_const_pointer_return(pc);
  take_volatile_pointer_return(p);
  take_volatile_pointer_return(pv);
  take_restrict_pointer_return(p);
  take_restrict_pointer_return(pr);
  take_optional_pointer_return(p);
  take_optional_pointer_return(po);
}

/* Matching pointee-qualified return types remain compatible. These qualifiers
   are part of the referenced type, unlike the ignored return qualifiers above. */
void test_pointee_controls(ConstPointeeReturnFunc *pc,
                          VolatilePointeeReturnFunc *pv,
                          OptionalPointeeReturnFunc *po,
                          RestrictPointeeReturnFunc *pr)
{
  take_const_pointee_return(pc);
  take_volatile_pointee_return(pv);
  take_optional_pointee_return(po);
  take_restrict_pointee_return(pr);
}

/* Supply functions returning pointers to qualified targets where the expected
   function returns pointers to unqualified targets. This discards qualifiers,
   the opposite of return covariance. Restrict uses a pointer target. */
void test_reverse(ConstPointeeReturnFunc *pc, VolatilePointeeReturnFunc *pv,
                  OptionalPointeeReturnFunc *po, RestrictPointeeReturnFunc *pr)
{
  take_pointer_return(pc); /* WARNING */
  take_pointer_return(pv); /* WARNING */
  take_pointer_return(po); /* WARNING */
  take_pointer_pointer_return(pr); /* WARNING */
}
#endif

#if TEST11 || TEST15
/* Shared function types for matching and contravariant parameter-target qualification. */
#if __has_reentrant
#define REENTRANT __reentrant
#else
#define REENTRANT
#endif
typedef void PlainParamFunc(char **) REENTRANT;
PlainParamFunc plain_param;
void take_plain_param(PlainParamFunc *);
typedef void ConstPointeeParamFunc(char *const *) REENTRANT;
ConstPointeeParamFunc const_pointee_param;
void take_const_pointee_param(ConstPointeeParamFunc *);
typedef void VolatilePointeeParamFunc(char *volatile *) REENTRANT;
VolatilePointeeParamFunc volatile_pointee_param;
void take_volatile_pointee_param(VolatilePointeeParamFunc *);
typedef void RestrictPointeeParamFunc(char *restrict *) REENTRANT;
RestrictPointeeParamFunc restrict_pointee_param;
void take_restrict_pointee_param(RestrictPointeeParamFunc *);
typedef void OptionalPointeeParamFunc(char *_Optional *) REENTRANT;
OptionalPointeeParamFunc optional_pointee_param;
void take_optional_pointee_param(OptionalPointeeParamFunc *);

#endif

#ifdef TEST11
/* Parameter-target qualifiers must match. The main function groups matching controls
   and pointer-value warnings. Designator checks need separate functions because error 78
   stops checking that function: volatile/_Optional on all targets, const/restrict on PDK. */
void test(PlainParamFunc *p,
          ConstPointeeParamFunc *pc,
          VolatilePointeeParamFunc *pv,
          RestrictPointeeParamFunc *pr,
          OptionalPointeeParamFunc *po)
{
  take_plain_param(p);
  take_plain_param(plain_param);
  take_const_pointee_param(pc);
  take_const_pointee_param(const_pointee_param);
  take_volatile_pointee_param(pv);
  take_volatile_pointee_param(volatile_pointee_param);
  take_restrict_pointee_param(pr);
  take_restrict_pointee_param(restrict_pointee_param);
  take_optional_pointee_param(po);
  take_optional_pointee_param(optional_pointee_param);

  /* These conversions discard parameter-target qualifiers. */
  take_const_pointee_param(p); /* WARNING */
  take_volatile_pointee_param(p); /* WARNING */
  take_restrict_pointee_param(p); /* WARNING */
  take_optional_pointee_param(p); /* WARNING */
}

/* These functions take pointers to unqualified targets, but the expected
   function types take pointers to qualified targets. This is the opposite
   of parameter contravariance; N3449 would not permit these conversions.
   SDCC uses error 78 for volatile/_Optional designators, and for const/restrict
   designators on PDK; the other cases use warning 244. ISO C requires a
   diagnostic, without prescribing these differences in severity. */
void test_reverse_const_designator(void)
{
  take_const_pointee_param(plain_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}

void test_reverse_volatile_designator(void)
{
  take_volatile_pointee_param(plain_param); /* ERROR */
}

void test_reverse_restrict_designator(void)
{
  take_restrict_pointee_param(plain_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}

void test_reverse_optional_designator(void)
{
  take_optional_pointee_param(plain_param); /* ERROR */
}

#endif

#if TEST12 || TEST16
/* Shared function types for matching and covariant return-target qualification. */
#if __has_reentrant
#define REENTRANT __reentrant
#else
#define REENTRANT
#endif
/* Work around SDCC bug #4104: direct char ** function-return typedefs
   with __reentrant are mishandled. */
typedef char *CharPointer;
typedef CharPointer *PlainReturnFunc(void) REENTRANT;
PlainReturnFunc plain_return;
void take_plain_return(PlainReturnFunc *);
typedef CharPointer const *ConstPointeeReturnFunc(void) REENTRANT;
ConstPointeeReturnFunc const_pointee_return;
void take_const_pointee_return(ConstPointeeReturnFunc *);
typedef CharPointer volatile *VolatilePointeeReturnFunc(void) REENTRANT;
VolatilePointeeReturnFunc volatile_pointee_return;
void take_volatile_pointee_return(VolatilePointeeReturnFunc *);
typedef CharPointer restrict *RestrictPointeeReturnFunc(void) REENTRANT;
RestrictPointeeReturnFunc restrict_pointee_return;
void take_restrict_pointee_return(RestrictPointeeReturnFunc *);
typedef CharPointer _Optional *OptionalPointeeReturnFunc(void) REENTRANT;
OptionalPointeeReturnFunc optional_pointee_return;
void take_optional_pointee_return(OptionalPointeeReturnFunc *);

#endif

#ifdef TEST12
/* Referenced return qualifiers must match. Matching controls and qualifier-loss conversions
   are grouped; the invalid directions remain invalid under N3449. */
void test(PlainReturnFunc *p,
          ConstPointeeReturnFunc *pc,
          VolatilePointeeReturnFunc *pv,
          RestrictPointeeReturnFunc *pr,
          OptionalPointeeReturnFunc *po)
{
  take_plain_return(p);
  take_plain_return(plain_return);
  take_const_pointee_return(pc);
  take_const_pointee_return(const_pointee_return);
  take_volatile_pointee_return(pv);
  take_volatile_pointee_return(volatile_pointee_return);
  take_restrict_pointee_return(pr);
  take_restrict_pointee_return(restrict_pointee_return);
  take_optional_pointee_return(po);
  take_optional_pointee_return(optional_pointee_return);

  /* Discarding referenced qualifiers remains invalid under N3449. */
  take_plain_return(const_pointee_return); /* WARNING */
  take_plain_return(volatile_pointee_return); /* WARNING */
  take_plain_return(restrict_pointee_return); /* WARNING */
  take_plain_return(optional_pointee_return); /* WARNING */
  take_plain_return(pc); /* WARNING */
  take_plain_return(pv); /* WARNING */
  take_plain_return(pr); /* WARNING */
  take_plain_return(po); /* WARNING */
}


#endif

#ifdef TEST13
/* Innermost target qualifiers remain significant through multiple pointer levels. Both
   addition and removal are incompatible; matching types provide valid controls. */
void take_plain(int ***);
void take_const(const int ***);
void take_volatile(volatile int ***);
void take_optional(_Optional int ***);
void take_pointer(int ****);
void take_restrict(int *restrict ***);
void test(int ***p, const int ***pc, volatile int ***pv,
          _Optional int ***poi, int ****pp, int *restrict ***pr)
{
  take_plain(pc); /* WARNING */
  take_plain(pv); /* WARNING */
  take_plain(poi); /* WARNING */
  take_const(p); /* WARNING */
  take_volatile(p); /* WARNING */
  take_optional(p); /* WARNING */
  take_pointer(pr); /* WARNING */
  take_restrict(pp); /* WARNING */
  take_plain(p);
  take_const(pc);
  take_volatile(pv);
  take_optional(poi);
  take_pointer(pp);
  take_restrict(pr);
}
#endif

#ifdef TEST14
/* A pointer-to-array is incompatible with a pointer-to-pointer. Separate functions
   distinguish these mismatches from valid outer array decay, address-of and void conversions. */
void take_pointer(int **);
void take_array(int *(*p)[2]);
void take_const_array(const int *(*p)[2]);
void test(int **p, int *(*a)[2], const int *(*ca)[2])
{
  take_pointer(a); /* WARNING */
  take_pointer(ca); /* WARNING */
  take_array(p); /* WARNING */
  take_const_array(p); /* WARNING */
  take_pointer(p);
  take_array(a);
  take_const_array(ca);
}

void take_void(void *);
void test_array_decay(void)
{
  int *a[2] = {0};
  take_pointer(a); /* The outer array expression decays to int **. */
  take_array(&a); /* Address-of preserves the array as the referenced type. */
  take_void(a);
  take_void(&a);
}
#endif

/* Conversions whose required diagnostics would change under N3449. */

#ifdef TEST15
/* C23 requires compatible parameter types; N3449 would permit these contravariant
   conversions. Designator checks need separate functions because PDK error 78 stops
   checking that function. Pointer-value conversions can be grouped. */
void test_variance_const_designator(void)
{
  take_plain_param(const_pointee_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}

void test_variance_volatile_designator(void)
{
  take_plain_param(volatile_pointee_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}

void test_variance_restrict_designator(void)
{
  take_plain_param(restrict_pointee_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}

void test_variance_optional_designator(void)
{
  take_plain_param(optional_pointee_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}

void test_variance(ConstPointeeParamFunc *pc,
          VolatilePointeeParamFunc *pv,
          RestrictPointeeParamFunc *pr,
          OptionalPointeeParamFunc *po)
{
  take_plain_param(pc); /* WARNING */
  take_plain_param(pv); /* WARNING */
  take_plain_param(pr); /* WARNING */
  take_plain_param(po); /* WARNING */
}
#endif

#ifdef TEST16
/* C23 requires compatible return types; N3449 would permit these covariant conversions.
   Const/restrict designators and pointer values are grouped. Volatile/_Optional
   designators need separate functions because error 78 stops checking that function. */
/* SDCC emits error 78 for this function designator conversion, whereas
   the const/restrict cases and function-pointer arguments emit warning 244.
   ISO C requires a diagnostic; it does not require this severity difference. */
void test_variance_volatile_designator(void)
{
  take_volatile_pointee_return(plain_return); /* ERROR */
}

/* This designator conversion also emits error 78 rather than warning 244. */
void test_variance_optional_designator(void)
{
  take_optional_pointee_return(plain_return); /* ERROR */
}

void test_variance(PlainReturnFunc *p)
{
  take_const_pointee_return(plain_return); /* WARNING */
  take_restrict_pointee_return(plain_return); /* WARNING */

  take_const_pointee_return(p); /* WARNING */
  take_volatile_pointee_return(p); /* WARNING */
  take_restrict_pointee_return(p); /* WARNING */
  take_optional_pointee_return(p); /* WARNING */
}
#endif


#ifdef TEST17
/* Check parameter contravariance with multiple parameters, including their order. C23
   rejects these conversions; N3449 would permit them. Designator checks need separate
   functions because PDK error 78 stops checking that function; pointer values are grouped. */
void test_const_designator(void)
{
  take_plain_param(const_pointee_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}
void test_volatile_designator(void)
{
  take_plain_param(volatile_pointee_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}
void test_optional_designator(void)
{
  take_plain_param(optional_pointee_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}
void test_restrict_designator(void)
{
  take_pointer_pointee_param(restrict_pointee_param); /* ERROR(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15) */ /* WARNING(!(SDCC_pdk13 || SDCC_pdk14 || SDCC_pdk15)) */
}
void test(ConstPointeeParamFunc *pc, VolatilePointeeParamFunc *pv,
          OptionalPointeeParamFunc *po, RestrictPointeeParamFunc *pr)
{
  take_plain_param(pc); /* WARNING */
  take_plain_param(pv); /* WARNING */
  take_plain_param(po); /* WARNING */
  take_pointer_pointee_param(pr); /* WARNING */

  take_const_pointee_param(pc);
  take_volatile_pointee_param(pv);
  take_optional_pointee_param(po);
  take_restrict_pointee_param(pr);
}
#endif

#ifdef TEST18
/* C23 rejects these return-covariant qualification additions; N3449 would permit them. */
void test_covariant(PointerReturnFunc *p, PointerPointerReturnFunc *pp)
{
  take_const_pointee_return(p); /* WARNING */
  take_volatile_pointee_return(p); /* WARNING */
  take_optional_pointee_return(p); /* WARNING */
  take_restrict_pointee_return(pp); /* WARNING */
}
#endif
