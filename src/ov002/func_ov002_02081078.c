#include "types.h"

extern u8 *data_ov002_02102070;
extern u32 func_ov002_02080edc(u32, u32);

void func_ov002_02081078(u32 a0, u32 a1)
{
    if (*(u8 *)(data_ov002_02102070 + a0 + 0x3d) == 0)
        func_ov002_02080edc(a0, a1);
}