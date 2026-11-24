#include "types.h"

extern u32 func_0201cde8(u32, u32, u32, u32);

typedef struct {
    u8 pad_0[280];
    u32 f0x118;
} S_0201f26c;

u32 func_0201f26c(u32 a0, u32 a1, u32 a2)
{
    u32 t0;
    if (((S_0201f26c *)a0)->f0x118 == 0x0) {
        t0 = func_0201cde8(a0, a1, a2, ((S_0201f26c *)a0)->f0x118);
        if (t0 == 0x0) return 0x0;
    }
    ((S_0201f26c *)a0)->f0x118 = (((S_0201f26c *)a0)->f0x118 + 0x1);
    return 0x1;
}