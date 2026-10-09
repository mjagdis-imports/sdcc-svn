/* Invalid derived underlying types are checked in valdiag. These integer
   typedef cases already pass on the baseline. */
#include <testfwk.h>
#ifdef __SDCC
#pragma std_c23
#endif

#ifdef __SDCC
/* A chain of integer typedefs is a valid underlying type, including
   when the enum definition follows a forward declaration. */
typedef int I;
typedef I J;
typedef unsigned char C;
enum E : J;
enum E : J
{
  low = -2, high = 3
};
enum B : C
{
  byte_max = 255
};
#endif
void
testUnderlyingTypedefs (void)
{
#ifdef __SDCC
  /* Resolving the typedef chain must retain negative and positive values. */
  volatile enum E e = low;
  volatile enum B b = byte_max;
  ASSERT (e == -2);
  e = high;
  ASSERT (e == 3);
  /* The unsigned-char underlying type must retain its full value range. */
  ASSERT (b == 255);
  /* Both enum layouts must match their respective underlying types. */
  ASSERT (sizeof (enum E) == sizeof (J));
  ASSERT (sizeof (enum B) == sizeof (C));
#endif
}
