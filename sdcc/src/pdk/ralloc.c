#include "ralloc.h"
#include "gen.h"

/*-----------------------------------------------------------------*/
/* assignRegisters - assigns registers to each live range as need  */
/*-----------------------------------------------------------------*/
void
pdk_assignRegisters (ebbIndex *ebbi)
{
  eBBlock **ebbs = ebbi->bbOrder;
  int count = ebbi->count;

  iCode *ic = iCodeLabelOptimize (iCodeFromeBBlock (ebbs, count));

  genPdkCode (ic);
}

