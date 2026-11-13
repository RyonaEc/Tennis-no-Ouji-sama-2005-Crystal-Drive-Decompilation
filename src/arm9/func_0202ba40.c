#include "types.h"

typedef struct {
    u32 f0x0;
    u8 pad_4[4];
    u32 f0x8;
} S_0202ba40;

u32 func_0202ba40(u32 a0, u32 a1)
{
    if (a0 != 0x0) {
        ((S_0202ba40 *)a1)->f0x8 = ((S_0202ba40 *)a0)->f0x0;
        ((S_0202ba40 *)a0)->f0x0 = a1;
    }
    return a0;
}
