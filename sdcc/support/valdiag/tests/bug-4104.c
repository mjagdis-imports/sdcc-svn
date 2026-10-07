#pragma std_c11

#if !__has_reentrant && !defined(__SDCC_pdk13) && \
    !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15)
#define __reentrant
#endif

typedef char **PointerReturnFunction(void) __reentrant;

#ifdef TEST1
PointerReturnFunction *pointer_return_function;
void test(void)
{
  pointer_return_function = 0;
}
#endif

#ifdef TEST2
typedef char ***TriplePointerReturnFunction(void) __reentrant;
TriplePointerReturnFunction *triple_pointer_return_function;
void test(void)
{
  triple_pointer_return_function = 0;
}
#endif

#ifdef TEST3
typedef const char *const *QualifiedReturnFunction(void) __reentrant;
typedef const char *const *(*QualifiedReturnPointer)(void) __reentrant;
_Static_assert(_Generic((QualifiedReturnFunction *)0,
                        QualifiedReturnPointer: 1, default: 0),
               "qualified return type");
#endif

#ifdef TEST4
#if __has_xdata
typedef char __xdata **AddressSpaceReturnFunction(void) __reentrant;
AddressSpaceReturnFunction *address_space_return_function;
void test(void)
{
  address_space_return_function = 0;
}
#endif
#endif
