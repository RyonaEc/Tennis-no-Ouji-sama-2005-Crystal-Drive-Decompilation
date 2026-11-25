#include "types.h"

typedef struct {
    u32 f0;
    u32 f4;
    u8 pad8[0x10];
    u32 f18;
    u32 f1c;
    u32 f20;
    u32 f24;
    u32 f28;
    u8 pad2c[0x14];
    u32 f40;
    u8 pad44[4];
    u32 f48;
} S_02034154;

extern void func_020341f4(S_02034154 *p);
extern void func_0203422c(u32 a0, u32 *a1);

u32 func_02034154(S_02034154 *p, u32 *out)
{
    u32 n = p->f24 - p->f1c;
    u32 ret;

    if (n != 0) {
        p->f28 = n;
        if (((p->f4 << 19) >> 31) == 0)
            func_0203422c(p->f1c, &p->f28);
        ret = ((u32 (*)(u32, u32, u32 *, u32))p->f40)(p->f0, p->f1c, &p->f28, p->f48);
        if (out != 0)
            *out = p->f28;
        if (ret != 0)
            return ret;
        p->f18 = p->f18 + p->f28;
    }
    func_020341f4(p);
    return 0;
}