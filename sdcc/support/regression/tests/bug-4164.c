/* Qualifiers on an array typedef must qualify its pointer elements.
   All assertions below fail without the fix except the integer-array
   preservation check, which already passes. */
#include <testfwk.h>

typedef int *ArrayOfPointersToInt[3];
typedef int *const ArrayOfConstPointersToInt[3];
typedef int *volatile ArrayOfVolatilePointersToInt[3];
typedef int *restrict ArrayOfRestrictPointersToInt[3];
typedef int *const volatile ArrayOfConstVolatilePointersToInt[3];

typedef ArrayOfPointersToInt MatrixOfPointersToInt[2];
typedef ArrayOfConstPointersToInt MatrixOfConstPointersToInt[2];

typedef int **ArrayOfPointersToPointersToInt[3];
typedef int **const ArrayOfConstPointersToPointersToInt[3];
typedef const int *ArrayOfPointersToConstInt[3];
typedef const int *volatile ArrayOfVolatilePointersToConstInt[3];

typedef int ArrayOfInt[3];
typedef const int ArrayOfConstInt[3];

#ifdef __SDCC
typedef int * _Optional ArrayOfOptionalPointersToInt[3];
typedef _Optional int *ArrayOfPointersToOptionalInt[3];
typedef _Optional int *const ArrayOfConstPointersToOptionalInt[3];
#endif

void
testArrayTypedefQualifiers(void)
{
  /* Without the fix, const and volatile qualify the referenced int
     instead of the array's pointer elements. */
  ASSERT (_Generic ((const ArrayOfPointersToInt *)0,
                    ArrayOfConstPointersToInt *: 1, default: 0));
  ASSERT (_Generic ((volatile ArrayOfPointersToInt *)0,
                    ArrayOfVolatilePointersToInt *: 1, default: 0));
  ASSERT (_Generic ((const volatile ArrayOfPointersToInt *)0,
                    ArrayOfConstVolatilePointersToInt *: 1, default: 0));

  /* Without the fix, const after the typedef name also qualifies the
     referenced int instead of the pointer elements. */
  ASSERT (_Generic ((ArrayOfPointersToInt const *)0,
                    ArrayOfConstPointersToInt *: 1, default: 0));

  /* Without the fix, restrict qualifies the referenced int instead of
     the pointer elements. */
  ASSERT (_Generic ((restrict ArrayOfPointersToInt *)0,
                    ArrayOfRestrictPointersToInt *: 1, default: 0));

  /* Without the fix, const qualifies the referenced int even with
     multiple array dimensions. It must qualify the pointer elements. */
  ASSERT (_Generic ((const MatrixOfPointersToInt *)0,
                    MatrixOfConstPointersToInt *: 1, default: 0));

  /* Without the fix, const qualifies int instead of the pointer elements
     that point to pointers to int. */
  ASSERT (_Generic ((const ArrayOfPointersToPointersToInt *)0,
                    ArrayOfConstPointersToPointersToInt *: 1, default: 0));
  /* Without the fix, volatile is added to the const int instead of
     the pointer elements. The existing const must be preserved. */
  ASSERT (_Generic ((volatile ArrayOfPointersToConstInt *)0,
                    ArrayOfVolatilePointersToConstInt *: 1, default: 0));

  /* Preservation: this already passes without the fix. Arrays of integer
     elements retain their existing qualification rules. */
  ASSERT (_Generic ((const ArrayOfInt *)0,
                    ArrayOfConstInt *: 1, default: 0));

#ifdef __SDCC
  /* Without the fix, qualifying the array typedef moves _Optional to the
     pointer element's referenced type. Check the constructed type here. */
  ASSERT (_Generic ((_Optional ArrayOfPointersToInt *)0,
                    ArrayOfOptionalPointersToInt *: 1, default: 0));
  /* Without the fix, const is added to the optional referenced int rather
     than to the pointer element. The existing _Optional must be preserved. */
  ASSERT (_Generic ((const ArrayOfPointersToOptionalInt *)0,
                    ArrayOfConstPointersToOptionalInt *: 1, default: 0));
#endif
}
