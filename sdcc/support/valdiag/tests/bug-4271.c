// A comparison with a non-null pointer can prove a pointer to _Optional non-null.

static int object;

#ifdef TEST1
void equal_address(_Optional int *p)
{
    if (p == &object)
        *p = 2;
}
#endif

#ifdef TEST2
void equal_after_return(_Optional int *p)
{
    if (p != &object)
        return;
    *p = 2;
}
#endif

#ifdef TEST3
void different_address(_Optional int *p)
{
    if (p != &object)
        *p = 2; /* WARNING */
}
#endif

#ifdef TEST4
void equal_unknown(_Optional int *p, _Optional int *q)
{
    if (p == q)
        *p = 2; /* WARNING */
}
#endif

#ifdef TEST5
static _Optional int *volatile p;

void volatile_pointer(void)
{
    if (p == &object)
        *p = 2; /* WARNING */
}
#endif

#ifdef TEST6
void joined_paths(_Optional int *p)
{
    if (p == &object)
        *p = 2;
    *p = 3; /* WARNING */
}
#endif

#ifdef TEST7
void equal_checked_pointer(_Optional int *p, _Optional int *q)
{
    if (!q)
        return;
    if (q == p)
        *p = 2;
}
#endif

#ifdef TEST8
void short_circuit_and(_Optional int *p, _Bool flag)
{
    if (flag && (p == &object))
        *p = 2;
}
#endif

#ifdef TEST9
void short_circuit_or(_Optional int *p, _Bool flag)
{
    if (flag || (p == &object))
        *p = 2; /* WARNING */
}
#endif

#ifdef TEST10
void constant_false_or(_Optional int *p)
{
    if (0 || (p == &object))
        *p = 2;
}
#endif

#ifdef TEST11
void comparison_first_and(_Optional int *p, _Bool flag)
{
    if ((p == &object) && flag)
        *p = 2;
}
#endif

#ifdef TEST12
void comparison_first_or(_Optional int *p, _Bool flag)
{
    if ((p == &object) || flag)
        *p = 2; /* WARNING */
}
#endif

#ifdef TEST13
void comparison_first_constant_false_or(_Optional int *p)
{
    if ((p == &object) || 0)
        *p = 2;
}
#endif

#ifdef TEST14
void constant_true_and(_Optional int *p)
{
    if (1 && (p == &object))
        *p = 2;
}
#endif

#ifdef TEST15
void comparison_first_constant_true_and(_Optional int *p)
{
    if ((p == &object) && 1)
        *p = 2;
}
#endif

#ifdef TEST16
#pragma disable_warning 126
void constant_true_or(_Optional int *p)
{
    if (1 || (p == &object))
        *p = 2; /* WARNING */
}
#endif

#ifdef TEST17
#pragma disable_warning 110
void comparison_first_constant_true_or(_Optional int *p)
{
    if ((p == &object) || 1)
        *p = 2; /* WARNING */
}
#endif
