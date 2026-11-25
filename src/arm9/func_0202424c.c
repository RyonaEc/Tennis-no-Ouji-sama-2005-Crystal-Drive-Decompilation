#include "types.h"

extern u32 data_02048cac;
extern u32 data_02048cbc;
extern u32 func_020207f4(void);

void func_0202424c(void)
{
    u32 mode = func_020207f4();

    data_02048cac = mode;

    switch (mode) {
    case 2:
    case 3:
        data_02048cbc = 0x06800000;
        break;
    case 8:
    case 12:
        data_02048cbc = 0x06840000;
        break;
    case 1:
        data_02048cbc = 0x067e0000;
        break;
    case 4:
        data_02048cbc = 0x06820000;
        break;
    }
}