#include "types.h"

extern u8 *data_ov008_0204f440[];

extern u32 func_02029c8c(void *a0);
extern s32 func_02029930(void *a0, u32 a1);
extern u32 func_02029828(void *a0, void *a1, u32 a2);
extern s32 func_020298e4(void *a0);
extern void func_02025c94(u32 a0, u32 a1);
extern void func_02026cc0(const void *srcp, void *destp);

u32 func_ov008_0204f144(u32 a0, u32 a1, u32 a2)
{
    u8 *p = (u8 *)data_ov008_0204f440[0] + 0x1154;
    u8 t[0x44];
    u32 len;

    func_02029c8c(t);
    if (func_02029930(t, a0) == 0)
        return 0;
    len = *(u32 *)(t + 0x24) - *(u32 *)(t + 0x20);
    if (len > 0xa000) {
        func_020298e4(t);
        return len;
    }
    func_02029828(t, p, len);
    func_020298e4(t);
    func_02025c94((u32)p, len);
    len = *(u32 *)p >> 8;
    if (len > a2)
        return len;
    func_02026cc0(p, (void *)a1);
    func_02025c94(a1, len);
    return len;
}