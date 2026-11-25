#include "types.h"

#define HI(d) (*(1 + (s32 *)&(d)))

double func_0203684c(double x)
{
    HI(x) = HI(x) & 0x7fffffff;
    return x;
}