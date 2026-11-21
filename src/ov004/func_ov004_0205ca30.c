#include "types.h"

extern u32 data_ov004_020690a8;

extern void func_ov004_020520b8(u8 *a0, u8 *a1, u8 *a2, s32 a3, s32 a4, s32 a5, s32 a6);
extern u8 data_ov004_020646b4[];

void func_ov004_0205ca30(void)
{
    func_ov004_020520b8((u8 *)data_ov004_020690a8 + 0x3630,
                        data_ov004_020646b4, data_ov004_020646b4,
                        0x1f, 0xf, 1, 1);
}