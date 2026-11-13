#include "types.h"

typedef struct {
    u32 f0x0;
} S_0201c4a0;

u32 func_0201c4a0(u32 a0, u32 a1)
{
    u32 v0;
    v0 = ((S_0201c4a0 *)a0)->f0x0;
    if (((S_0201c4a0 *)a0)->f0x0 != 0x0) {
        *(u8 *)((u8 *)(((S_0201c4a0 *)a0)->f0x0) + 64) = a1;
    }
    return v0;
}
