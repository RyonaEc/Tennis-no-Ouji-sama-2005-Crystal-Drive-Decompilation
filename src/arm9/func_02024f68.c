#include "types.h"

typedef struct {
    u8 pad_0[112];
    u32 f0x70;
} S_02024f68;

u32 func_02024f68(u32 a0)
{
    return ((S_02024f68 *)a0)->f0x70;
}
