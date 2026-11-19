#include "types.h"

typedef void (*Fn_02019d64)(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);

extern u8 data_02046630[];
extern u8 data_0204662c[];
extern u8 data_02042360[];

void func_02019dc4(u32 a0, u32 a1, u32 a2)
{
    u8 *p = *(u8 **)data_02046630;
    Fn_02019d64 fn = *(Fn_02019d64 *)(p + 0x44);

    if (fn == 0)
        return;
    fn(*(u32 *)data_0204662c, (u32)p, a1, a2, *(u32 *)data_02042360);
}