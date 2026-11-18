#include "types.h"

extern u8 data_ov008_0204f440[];
extern u8 *func_ov008_0204ed50(void);
extern u8 *func_ov008_0204ed3c(void);

void func_ov008_0204ded8(void)
{
    u8 *base = *(u8 **)data_ov008_0204f440;
    u8 *p;

    p = func_ov008_0204ed50();
    *(u32 *)p |= 0x800;
    *(u16 *)(p + 0x12) = *(u16 *)(base + 0x36);
    *(u16 *)(p + 0x20) = 0;
    *(u16 *)(p + 0x22) = 0;
    p = func_ov008_0204ed3c();
    *(u32 *)p |= 0x800;
    *(u16 *)(p + 0x12) = *(u16 *)(base + 0x38);
    *(u16 *)(p + 0x20) = 0;
    *(u16 *)(p + 0x22) = 0;
}