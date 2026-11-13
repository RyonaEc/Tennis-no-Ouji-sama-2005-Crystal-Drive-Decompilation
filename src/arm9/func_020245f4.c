#include "types.h"

extern u32 func_0202081c(void);
extern u8 data_02041a18[];
extern u8 data_02041a1a[];
extern u8 data_02041a1c[];
extern u8 data_02048cb0[];
extern u8 data_02048cc0[];
extern u8 data_02048cc4[];
extern u8 data_02048cc8[];

u32 func_020245f4(void)
{
    u32 t0;
    t0 = func_0202081c();
    *(u32 *)(data_02048cc0) = t0;
    *(u32 *)(data_02048cb0) = (*(u16 *)((u8 *)((data_02041a18 + (t0 * 0x6)))) << 0xc);
    *(u32 *)(data_02048cc4) = (*(u16 *)((u8 *)((data_02041a1a + (t0 * 0x6)))) << 0xc);
    *(u32 *)(data_02048cc8) = (*(u16 *)((u8 *)((data_02041a1c + (t0 * 0x6)))) << 0xc);
    return t0;
}
