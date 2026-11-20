#include "types.h"

extern s32 func_02020808(void);
extern u16 data_02041a08[];
extern u8 data_02048cb8[];
extern u8 data_02048cb4[];

void func_020243e0(void)
{
    s32 v = func_02020808();
    u32 x = data_02041a08[v >> 4];
    *(u32 *)data_02048cb8 = v;
    *(u32 *)data_02048cb4 = x << 12;
}