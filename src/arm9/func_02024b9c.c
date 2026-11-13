#include "types.h"

typedef struct {
    u8 pad_0[4];
    u16 f0x4;
} S_02024b9c;

u32 func_02024b9c(u32 a0)
{
    return ((S_02024b9c *)a0)->f0x4;
}
