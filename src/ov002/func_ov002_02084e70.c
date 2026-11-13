#include "types.h"

extern u32 func_ov002_02085940(u32);
extern u32 func_ov002_02080b70(u32);
extern u32 func_ov002_02084ff4(u32);

u32 func_ov002_02084e70(u32 a0)
{
    u32 t0 = func_ov002_02085940((a0 & 0xff));
    u32 t1 = func_ov002_02080b70(t0);
    u32 t2 = func_ov002_02084ff4(t1);
    return t2;
}
