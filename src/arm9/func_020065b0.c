#include "types.h"

typedef struct {
    u8 *p0;
    u8 *p4;
    u8 *p8;
} S;

extern volatile S data_02042a68;
extern void func_0201c524(u32);
extern s32 func_0201e014(u32, u32);
extern void func_0201d900(u32, u32);
extern void func_0201e670(u32, u32);

void func_020065b0(u32 a0, u32 a1)
{
    u8 *p = data_02042a68.p8;

    if (a0 != 0) {
        if (a1 == *(u32 *)(p + 0xa0)) return;
    }
    if (*(u32 *)(p + 0x8c) != 0) func_0201c524((u32)(p + 0x8c));
    if (func_0201e014(a1, *(u32 *)p) == 0) {
        func_0201d900(*(u32 *)p, *(u32 *)(p + 0xb4));
        func_0201e014(a1, *(u32 *)p);
    }
    func_0201e670((u32)(p + 0x8c), a1);
    *(u32 *)(p + 0xa0) = a1;
}