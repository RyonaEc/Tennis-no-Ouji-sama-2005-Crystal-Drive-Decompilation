#include "types.h"

extern s32 func_ov002_0204f70c(s32, s32);
extern void func_ov002_0204f770(s32, u32, s32 *);
void func_ov002_0204fdb0(u8 *e, s16 a1, s16 a2)
{
    s32 out[3];
    s16 s = func_ov002_0204f70c((func_02034148(*(s32 *)(e + 0x60)) << 8) >> 16,
                                (*(s32 *)(e + 0x5c) << 8) >> 16);
    s16 d = s * 360 / 65536;
    s16 ang = a1 + d;
    s32 sg = *(s32 *)(e + 0x60) < 0 ? -1 : 1;
    func_ov002_0204f770(a2 * sg, (s16)(sg * ((ang << 8) / 360)) & 0xff, out);
    *(s32 *)(e + 0x68) = out[0];
    *(s32 *)(e + 0x6c) = out[1];
}
