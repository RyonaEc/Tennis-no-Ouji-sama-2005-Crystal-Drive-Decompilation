#include "types.h"

typedef struct {
    u32 f0x0;
} S_0201c524;

u32 func_0201c524(u32 a0)
{
    if (((S_0201c524 *)a0)->f0x0 != 0x0) {
        *(u32 *)((u8 *)(((S_0201c524 *)a0)->f0x0)) = 0x0;
        ((S_0201c524 *)a0)->f0x0 = 0x0;
    }
    return a0;
}
