#include "types.h"

extern u8 data_ov008_0204f440[];
extern u8 data_02042a68[];

void func_ov008_0204e554(void)
{
    u8 *p = *(u8 **)data_ov008_0204f440;
    u16 v = *(u16 *)(data_02042a68 + 0x34);
    u16 old = *(u16 *)p;

    *(u16 *)(p + 2) = v & (v ^ old);
    *(u16 *)p = v;
}