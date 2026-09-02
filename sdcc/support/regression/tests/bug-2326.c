/*
    bug-2326.c, Incorrect LUT retrieval index
*/

#include <testfwk.h>
#include <stdint.h>

const uint8_t lut[] = {
 // key,  val1, val2,
    0x12, 0xAB, 0xCD,
    0x34, 0xEF, 0x98,
    0x56, 0xC6, 0xDA,
    0x00, 0x00, 0x00, // end of table
};

// return 1 if entry is found, 0 if not found
static char findEntryBad(uint8_t key, uint8_t* val1, uint8_t* val2)
{
    uint8_t i, ii, j;
    for (i = 0; ; i++)
    {
        ii = i * 3; // table has 3 columns
        j = lut[ii + 0];
        if (j == key) // found
        {
            *val1 = lut[ii + 1];
            *val2 = lut[ii + 2];
            return 1;
        }
        else if (j == 0) // end of table
        {
            return 0;
        }
    }
    return 0;
}

// return 1 if entry is found, 0 if not found
static char findEntryGood(uint8_t key, uint8_t* val1, uint8_t* val2)
{
    volatile uint8_t i, ii, j; // volatile forces calculation of ii
    for (i = 0; ; i++)
    {
        ii = i * 3; // table has 3 columns
        j = lut[ii + 0];
        if (j == key) // found
        {
            *val1 = lut[ii + 1];
            *val2 = lut[ii + 2];
            return 1;
        }
        else if (j == 0) // end of table
        {
            return 0;
        }
    }
    return 0;
}

void testBug(void)
{
    uint8_t res, v1, v2;


    res = findEntryBad(0x12, &v1, &v2);
    ASSERT (v2 == 0xCD);

    res = findEntryGood(0x12, &v1, &v2);
    ASSERT (v2 == 0xCD);
}
