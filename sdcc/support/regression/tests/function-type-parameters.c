/* Function parameter types participate in exact type comparison. */
#include <testfwk.h>

#pragma disable_warning 361
typedef const int QualifiedReturn(void);
typedef int PlainReturn(void);
typedef QualifiedReturn *PQR;
typedef PlainReturn *PPR;

typedef int FI(int);
typedef int FF(float);
typedef int FCI(const int);
typedef int FPI(int *);
typedef int FCPI(const int *);
typedef FI *RI(void);
typedef FF *RF(void);

/* Complete pointer typedefs avoid the abstract-declarator discrepancy in #4107. */
typedef FI *PI;
typedef FF *PF;
typedef FCI *PCI;
typedef FPI *PPI;
typedef FCPI *PCPI;
typedef RI *PRI;
typedef RF *PRF;

void testFunctionTypeParameters(void)
{
  ASSERT(_Generic((PQR *)0, PPR *: 1, default: 0));
  ASSERT(_Generic((PPR *)0, PQR *: 1, default: 0));
  ASSERT(_Generic((PI *)0, PI *: 1, PF *: 2) == 1);
  ASSERT(_Generic((PF *)0, PI *: 1, PF *: 2) == 2);
  ASSERT(_Generic((PI *)0, PCI *: 1, default: 0) == 1);
  ASSERT(_Generic((PPI *)0, PCPI *: 1, default: 0) == 0);
  ASSERT(_Generic((PRI *)0, PRI *: 1, PRF *: 2) == 1);
}
