#include "types.h"

extern u32 data_02048c98;
extern u32 data_02048c94;
extern u32 func_02020830(void);

void func_020235a0(void)
{
    u32 r = func_02020830();

    data_02048c98 = r;
    if (r == 0)
        return;
    if (r != 0x20) {
        if (r == 0x40)
            data_02048c94 = 0x06894000;
        return;
    }
    data_02048c94 = 0x06890000;
}