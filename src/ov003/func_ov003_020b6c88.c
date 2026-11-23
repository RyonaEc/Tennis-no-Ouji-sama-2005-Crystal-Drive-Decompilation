#include "types.h"

extern u8 data_02042a68[];
extern u8 data_ov003_020eb720[];

void func_ov003_020b6c88(void)
{
    u16 f = *(u16 *)(data_02042a68 + 0x30);
    u8 *base = *(u8 **)data_ov003_020eb720;

    if (f & 2)
        return;
    if (!(f & 1))
        return;
    if (*(u16 *)(base + 0x86d6) == 0) {
        *(u16 *)(base + 0x86d6) = 1;
        *(u16 *)(base + 0x86ce) = 1;
        *(u16 *)(base + 0x86c2) = 0x22;
    }
}