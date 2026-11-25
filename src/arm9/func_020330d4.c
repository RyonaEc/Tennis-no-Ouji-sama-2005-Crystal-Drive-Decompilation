#include "types.h"

extern u32 func_02033c0c(void);
extern u32 func_02033bc0(void);
extern u32 func_02024c58(u32 a0);
extern u32 func_02024b9c(u32 a0);
extern void func_02033bd0(u32 f);
extern void func_02033c1c(u32 f);

u32 func_020330d4(u32 *out)
{
    u32 a = func_02033c0c();
    u32 b = func_02033bc0();
    u32 r = 0;
    u32 s = 0;

    if (func_02024c58(0x70) == 0) {
        s = 1;
        r = 1;
    } else if (func_02024b9c(0x027fffe8) & 0x40) {
        r = 1;
    }
    if (r != 0) {
        u32 f = 0;

        if (a != 0)
            f = f | 1;
        if (b != 0)
            f = f | 2;
        if (s != 0)
            f = f | 0x20;
        *out = f;
    } else {
        func_02033bd0(b);
        func_02033c1c(a);
        *out = 0;
    }
    return r;
}