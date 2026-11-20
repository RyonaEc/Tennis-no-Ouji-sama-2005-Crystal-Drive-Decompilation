#include "types.h"

extern u8 *func_ov008_0204ed50(void);
extern u8 *func_ov008_0204ed3c(void);

void func_ov008_0204e434(void)
{
    u8 *p;

    p = func_ov008_0204ed50();
    *(u32 *)p &= ~0xee00;
    *(u16 *)(p + 6) = 0;
    *(u16 *)(p + 8) = 0;
    *(u16 *)(p + 0xa) = 0;
    *(u8 *)(p + 0x24) = 0x3f;
    *(u8 *)(p + 0x25) = 0x3f;
    *(u8 *)(p + 0x26) = 0x3f;
    *(u8 *)(p + 0x27) = 0x3f;

    p = func_ov008_0204ed3c();
    *(u32 *)p &= ~0xee00;
    *(u16 *)(p + 6) = 0;
    *(u16 *)(p + 8) = 0;
    *(u16 *)(p + 0xa) = 0;
    *(u8 *)(p + 0x24) = 0x3f;
    *(u8 *)(p + 0x25) = 0x3f;
    *(u8 *)(p + 0x26) = 0x3f;
    *(u8 *)(p + 0x27) = 0x3f;
}