#include "types.h"

extern void func_0201fd50(s32 x);
extern s64 func_0201fdc8(void);
extern s64 func_0203af10(s64 a, s64 b);

#define FX32_ONE 0x1000
#define FX_Mul(a, b) ((s32)(((s64)(a) * (b) + 0x800) >> 12))
#define FX_Mul32x64c(a, b) ((s32)(((s64)(a) * (b) + 0x80000000LL) >> 32))

void func_02023018(s32 t, s32 b, s32 l, s32 r, s32 n, s32 f, s32 scaleW, u32 draw, s32 *mtx)
{
    s64 inv1, inv2;
    s32 dblN;
    s32 tmp1, tmp2;
    s32 *p;

    func_0201fd50(r - l);

    if (draw) {
        *(volatile u32 *)0x04000440 = 0;
        p = (s32 *)0x04000458;
    }
    if (mtx != 0) {
        mtx[1] = 0;
        mtx[2] = 0;
        mtx[3] = 0;
        mtx[4] = 0;
        mtx[6] = 0;
        mtx[7] = 0;
        mtx[0xb] = -FX32_ONE;
        mtx[0xc] = 0;
        mtx[0xd] = 0;
        mtx[0xf] = 0;
    }

    dblN = n << 1;

    inv1 = func_0201fdc8();

    {
        volatile u32 *divcnt = (volatile u32 *)0x04000290;
        volatile u32 *divnum = (volatile u32 *)0x04000298;

        divcnt[0] = 0;
        divcnt[1] = 0x1000;
        divnum[0] = (u32)(t - b);
        divnum[1] = 0;
    }

    if (scaleW != FX32_ONE)
        inv1 = func_0203af10(inv1 * scaleW, FX32_ONE);

    tmp1 = FX_Mul32x64c(inv1, dblN);
    if (draw) {
        *p = tmp1;
        *p = 0;
        *p = 0;
        *p = 0;
    }
    if (mtx != 0)
        mtx[0] = tmp1;

    inv2 = func_0201fdc8();

    {
        volatile u32 *divcnt = (volatile u32 *)0x04000290;
        volatile u32 *divnum = (volatile u32 *)0x04000298;

        divcnt[0] = 0;
        divcnt[1] = 0x1000;
        divnum[0] = (u32)(n - f);
        divnum[1] = 0;
    }

    if (scaleW != FX32_ONE)
        inv2 = func_0203af10(inv2 * scaleW, FX32_ONE);

    tmp1 = FX_Mul32x64c(inv2, dblN);
    if (draw) {
        *p = 0;
        *p = tmp1;
        *p = 0;
        *p = 0;
    }
    if (mtx != 0)
        mtx[5] = tmp1;

    tmp1 = FX_Mul32x64c(r + l, inv1);
    tmp2 = FX_Mul32x64c(t + b, inv2);
    if (draw) {
        *p = tmp1;
        *p = tmp2;
    }
    if (mtx != 0) {
        mtx[8] = tmp1;
        mtx[9] = tmp2;
    }

    inv1 = func_0201fdc8();

    if (scaleW != FX32_ONE)
        inv1 = func_0203af10(inv1 * scaleW, FX32_ONE);

    tmp1 = FX_Mul32x64c(f + n, inv1);
    tmp2 = FX_Mul32x64c(FX_Mul(dblN, f), inv1);

    if (draw) {
        *p = tmp1;
        *p = -scaleW;
        *p = 0;
        *p = 0;
        *p = tmp2;
        *p = 0;
    }
    if (mtx != 0) {
        mtx[0xa] = tmp1;
        mtx[0xe] = tmp2;
    }
}