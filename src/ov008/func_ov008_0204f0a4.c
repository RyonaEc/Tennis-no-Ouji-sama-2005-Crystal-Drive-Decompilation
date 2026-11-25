#include "types.h"

extern u32 *volatile data_ov008_0204f440;
extern u8 data_ov008_0204f400[];
extern void func_ov008_0204f144(void *a0, void *a1, u32 a2);

void func_ov008_0204f0a4(void)
{
    u8 *base = (u8 *)data_ov008_0204f440 + 0x1114;
    u32 *p;
    u32 *s;

    func_ov008_0204f144(data_ov008_0204f400, base + 0x23040, 0x400);
    p = (u32 *)(base + 0x23040);
    s = (u32 *)((u8 *)p + 4);
    *(u32 *)((u8 *)data_ov008_0204f440 + 0x1114) = (u32)((u8 *)p + s[1]);
    *(u32 *)((u8 *)data_ov008_0204f440 + 0x1118) = (u32)((u8 *)p + s[3]);
    *(u32 *)((u8 *)data_ov008_0204f440 + 0x111c) = (u32)((u8 *)p + s[5]);
    *(u32 *)((u8 *)data_ov008_0204f440 + 0x1120) = (u32)((u8 *)p + s[7]);
}