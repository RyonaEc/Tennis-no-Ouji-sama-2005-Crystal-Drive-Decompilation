#include "types.h"

extern u32 data_ov005_020c9744[];
extern u8 data_ov005_020c9644[];

extern u32 func_02029c8c(void *a0);
extern s32 func_02029930(void *a0, u32 a1);
extern void func_020263cc(void);
extern u32 func_02029828(void *a0, void *a1, u32 a2);
extern s32 func_020298e4(void *a0);
extern void func_02026cc0(const void *srcp, void *destp);
extern void func_02025c94(u32 a0, u32 a1);
extern u32 func_020126bc(u32 a0);

void func_ov005_0204f464(void)
{
    u8 *g = (u8 *)data_ov005_020c9744[0];
    u8 *src = g + 0x80000;
    u8 t[0x48];

    func_02029c8c(t);
    if (func_02029930(t, (u32)data_ov005_020c9644) == 0)
        func_020263cc();
    func_02029828(t, src, *(u32 *)(t + 0x24) - *(u32 *)(t + 0x20));
    func_020298e4(t);
    func_02026cc0(src, (u8 *)data_ov005_020c9744[0] + 0xe8000);
    func_02025c94((u32)((u8 *)data_ov005_020c9744[0] + 0xe8000), 0x20000);
    func_020126bc((u32)((u8 *)data_ov005_020c9744[0] + 0xe8000));
}