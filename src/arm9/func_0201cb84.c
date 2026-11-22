#include "types.h"

extern void func_02027244(u32 a0, u32 a1, u32 a2, u32 a3);
extern void func_0202b964(u8 *a0);
extern void func_0202b94c(u8 *a0);

typedef struct {
    s32 f0 : 1;
    s32 busy : 1;
    s32 rest : 30;
} Flags;

void func_0201cb84(u8 *a0)
{
    func_02027244(*(u32 *)(a0 + 0x44), 0, 1 << *(u32 *)(a0 + 0x40), 0);
    if (((Flags *)(a0 + 0x24))->busy != 0)
        return;
    func_0202b964(a0 + 8);
    func_0202b94c(a0 + 0x14);
    ((Flags *)(a0 + 0x24))->busy = 1;
}