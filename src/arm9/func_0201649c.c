#include "types.h"

extern u32 func_020176f0(u32);
extern u32 func_0201746c(u32);
extern u32 func_02016450(void);

typedef struct {
    u8 pad_0[2];
    u16 f0x2;
} S_0201649c;

u32 func_0201649c(u32 a0)
{
    u32 t0, t1, t2;
    if (((S_0201649c *)a0)->f0x2 != 0x0) {
        t0 = func_020176f0(((S_0201649c *)a0)->f0x2);
        t1 = func_0201746c(t0);
        return t1;
    }
    t2 = func_02016450();
    return t2;
}
