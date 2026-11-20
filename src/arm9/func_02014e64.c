#include "types.h"

extern void func_02015f5c(void);
extern u32 func_02015f40(u32 a0);
extern u32 func_0201745c(void);

u32 func_02014e64(u32 a0)
{
    u32 r;
    u32 v;

    r = 0;
    func_02015f5c();
    v = func_0201745c();
    switch (v) {
    case 7:
        r = 2;
        break;
    case 1:
        if (func_02015f40(a0) == 0)
            r = 2;
        break;
    case 5:
        r = 1;
        break;
    case 0:
    case 2:
    case 3:
    case 4:
    case 6:
        break;
    }
    return r;
}