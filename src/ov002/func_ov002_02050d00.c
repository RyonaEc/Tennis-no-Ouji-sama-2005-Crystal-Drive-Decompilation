#include "types.h"

extern void func_ov002_0204f770(s32, u32, s32 *);
void func_ov002_02050d00(u8 *e, s32 a1)
{
    s32 out[3];
    s32 x = *(s32 *)(e + 0x5c);
    s32 y = *(s32 *)(e + 0x60);
    u32 r = func_ov002_0204f6ac((x >> 8) * (x >> 8) + (y >> 8) * (y >> 8));
    func_ov002_0204f770(r << 8, (s16)((a1 << 8) / 360) & 0xff, out);
    *(s32 *)(e + 0x5c) = out[0];
    *(s32 *)(e + 0x60) = out[1];
}
