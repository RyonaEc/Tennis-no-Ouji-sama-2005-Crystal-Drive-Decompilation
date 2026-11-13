#include "types.h"

extern u32 func_020245f4(void);
extern u32 func_02024498(u32, u32, u32);
extern u32 func_02024428(u32);

u32 func_02018b60(u32 a0, u32 a1, u32 a2)
{
    u32 t0 = func_020245f4();
    u32 t1 = func_02024498(a0, a1, a2);
    return func_02024428(t1);
}
