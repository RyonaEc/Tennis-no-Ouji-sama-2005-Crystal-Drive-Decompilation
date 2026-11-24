#include "types.h"

/* NitroSDK VEC_Normalize (fx_vec.c). The SDK headers are restated as the
   register accesses and inline CP/OS helpers they expand to:
   CP_SetDiv64_64(1 << 56, t), CP_SetSqrt64(t << 2) with interrupts masked,
   then the div and sqrt results are multiplied back into the components. */
typedef s32 fx32;
typedef s64 fx64;

typedef struct { fx32 x, y, z; } VecFx32;

#define reg_CP_DIVCNT      (*(volatile u16 *)0x04000280)
#define reg_CP_DIV_NUMER   (*(volatile u64 *)0x04000290)
#define reg_CP_DIV_DENOM   (*(volatile u64 *)0x04000298)
#define reg_CP_DIV_RESULT  (*(volatile u64 *)0x040002a0)
#define reg_CP_SQRTCNT     (*(volatile u16 *)0x040002b0)
#define reg_CP_SQRT_RESULT (*(volatile u32 *)0x040002b4)
#define reg_CP_SQRT_PARAM  (*(volatile u64 *)0x040002b8)
#define reg_OS_IME         (*(volatile u16 *)0x04000208)

void func_0201fe0c(const VecFx32 * pSrc, VecFx32 * pDst)
{
    fx64 t;
    s32 sqrt;
    u32 ime;
    u32 bak;

    t  = (fx64) pSrc->x * pSrc->x;
    t += (fx64) pSrc->y * pSrc->y;
    t += (fx64) pSrc->z * pSrc->z;

    reg_CP_DIVCNT    = 2;
    reg_CP_DIV_NUMER = 1LL << 56;
    reg_CP_DIV_DENOM = (u64) t;

    ime = reg_OS_IME;
    reg_OS_IME = 0;
    reg_CP_SQRTCNT    = 1;
    reg_CP_SQRT_PARAM = (u64) (t << 2);
    bak = reg_OS_IME;
    reg_OS_IME = ime;

    while (reg_CP_SQRTCNT & 0x8000) { }
    sqrt = (s32) reg_CP_SQRT_RESULT;
    while (reg_CP_DIVCNT & 0x8000) { }
    t = (fx64) reg_CP_DIV_RESULT;

    t = t * sqrt;
    pDst->x = (fx32) ((t * pSrc->x + (1LL << 44)) >> 45);
    pDst->y = (fx32) ((t * pSrc->y + (1LL << 44)) >> 45);
    pDst->z = (fx32) ((t * pSrc->z + (1LL << 44)) >> 45);
}