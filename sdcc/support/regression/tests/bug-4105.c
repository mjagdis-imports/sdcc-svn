/* Leading pointers must preserve the arrays in abstract declarators.
   Pointer elements exercise the pointer direct_abstract_declarator rule. */
#include <testfwk.h>

/* Both forms must denote a pointer to an array of two int pointers.
   The faulty splice removed the array, producing int **. */
typedef int *Row[2];
typedef Row *RowPointer;
_Static_assert(_Generic((int *(*)[2])0, RowPointer: 1, default: 0),
               "pointer to array");
_Static_assert(sizeof(*(int *(*)[2])0) == sizeof(Row), "row size");

/* Both array dimensions must survive in the abstract pointer type.
   The faulty splice removed them, producing int **. */
typedef int *Matrix[2][3];
typedef Matrix *MatrixPointer;
_Static_assert(_Generic((int *(*)[2][3])0, MatrixPointer: 1, default: 0),
               "pointer to multidimensional array");
_Static_assert(sizeof(*(int *(*)[2][3])0) == sizeof(Matrix), "matrix size");

/* Adding another element pointer must preserve the array and pointer depth.
   The faulty splice removed the array, producing int ***. */
typedef int **DoublePointerRow[2];
typedef DoublePointerRow *DoublePointerRowPointer;
_Static_assert(_Generic((int **(*)[2])0, DoublePointerRowPointer: 1, default: 0),
               "array element pointer depth");

/* The array contains const pointers to int, not pointers to const int.
   The faulty splice removed the array containing those qualified elements. */
typedef int *const ConstPointerRow[2];
typedef ConstPointerRow *ConstPointerRowPointer;
_Static_assert(_Generic((int *const (*)[2])0, ConstPointerRowPointer: 1, default: 0),
               "qualified array element");

/* A pointer to an array of pointers to arrays must keep both array bounds.
   The faulty splice discarded array links in this nested declarator. */
typedef int *Inner[3];
typedef Inner *Outer[2];
typedef Outer *OuterPointer;
_Static_assert(_Generic((int *(*(*)[2])[3])0, OuterPointer: 1, default: 0),
               "nested pointers to arrays");

/* Adjusting the outer array parameter must leave a pointer to a three-element
   row. The faulty splice removed the inner array, producing int **. */
void adjusted(int *[2][3]);
void adjusted(int *p[2][3])
{
  (void)p;
}

/* The abstract type must be compatible with a pointer to an incomplete row. */
typedef int *(*IncompleteRowPointer)[];
_Static_assert(_Generic((int *(*)[])0, IncompleteRowPointer: 1, default: 0),
               "pointer to incomplete array");

/* The unnamed prototype must agree with the named definition. */
static int sumRow(int *(*)[2]);

/* Read indices at runtime so the accesses cannot all fold to constants. */
static volatile unsigned selectedRow = 1;
static volatile unsigned selectedColumn = 1;

void
testAbstractArrays(void)
{
  int a = 1;
  int b = 2;
  int c = 3;
  int d = 4;
  int *rows[2][2] = {{&a, &b}, {&c, &d}};
  int *const qualified[2] = {&a, &b};
  unsigned row = selectedRow;
  unsigned column = selectedColumn;

  /* Pointer arithmetic on the abstract cast type must advance by one row. */
  ASSERT((int *(*)[2])rows + row == &rows[1]);
  /* Both dynamic indices must reach the selected pointer element. */
  ASSERT(*((int *(*)[2])rows)[row][column] == d);
  /* The unnamed prototype must accept both the first and second row. */
  ASSERT(sumRow((int *(*)[2])rows) == a + b);
  ASSERT(sumRow((int *(*)[2])rows + row) == c + d);
  /* The matrix-pointer cast must preserve both array dimensions. */
  ASSERT(*(*(int *(*)[2][2])&rows)[row][column] == d);
  /* Indexing the qualified array must read the selected const pointer. */
  ASSERT((*(int *const (*)[2])&qualified)[column] == &b);
}

static int
sumRow(int *(*p)[2])
{
  return *(*p)[0] + *(*p)[1];
}
