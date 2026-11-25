#include "types.h"

extern double func_02037f0c(double a, double b);

#define HI(d) (*(1 + (s32 *)&(d)))
#define LO(d) (*(s32 *)&(d))

double func_02036874(double x, s32 *e)
{
    s32 hx;
    s32 ix;
    s32 lx;

    hx = HI(x);
    lx = LO(x);
    ix = hx & 0x7fffffff;
    *e = 0;
    if (ix >= 0x7ff00000 || (ix | lx) == 0)
        return x;
    if (ix < 0x00100000) {
        x = func_02037f0c(x, 18014398509481984.0);
        hx = HI(x);
        ix = hx & 0x7fffffff;
        *e = -54;
    }
    *e = *e + ((ix >> 20) - 0x3fe);
    HI(x) = (hx & 0x800fffff) | 0x3fe00000;
    return x;
}