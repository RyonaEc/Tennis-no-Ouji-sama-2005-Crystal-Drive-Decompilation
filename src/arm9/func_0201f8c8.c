#include "types.h"

typedef struct {
    u32 f0x0;
} S_0201f8c8;

u32 func_0201f8c8(u32 a0)
{
    if (((S_0201f8c8 *)a0)->f0x0 != 0x0) {
        *(u32 *)((u8 *)(((S_0201f8c8 *)a0)->f0x0) + 300) = 0x0;
        ((S_0201f8c8 *)a0)->f0x0 = 0x0;
    }
    return a0;
}
