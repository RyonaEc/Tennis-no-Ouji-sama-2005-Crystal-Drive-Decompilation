#include "types.h"

extern u8 data_0204aec0[];
extern u8 data_02048d3c[];
extern s32 func_02026350(u32);
extern void func_020251c0(u8 *);
extern void func_02026364(s32);
extern void func_0202cf10(void *);
extern void func_0202d3f0(u8 *);

s32 func_0202d22c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5)
{
    u32 *g = (u32 *)data_0204aec0;
    s32 v;

    v = func_02026350(a0);
    while (g[0xd] & 4)
        func_020251c0((u8 *)g + 0xd4);
    g[0xd] |= 4;
    func_02026364(v);
    g[6] = a0;
    g[7] = a1;
    g[8] = a2;
    g[0xa] = a3;
    g[0xb] = a4;
    if (a5 != 0) {
        func_0202cf10(func_0202d3f0);
        return 1;
    }
    *(u32 *)(data_0204aec0 + 0xd0) = *(u32 *)(data_02048d3c + 8);
    func_0202d3f0((u8 *)g);
    return *(u32 *)(*(u32 *)g) == 0;
}