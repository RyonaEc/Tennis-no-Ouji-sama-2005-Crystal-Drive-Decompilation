#include "types.h"

extern u32 func_0202cabc(u32, u32, u32);
extern u32 func_0202c430(u32, u32);
extern u8 data_0204ad00[];
extern u8 func_0202c448[];

u32 func_0202ca78(u32 a0)
{
    u32 t0, t1;
    t0 = func_0202cabc(a0, (u32)func_0202c448, 0x0);
    *(u32 *)(data_0204ad00 + 32) = t0;
    if (t0 == 0x0) {
        t1 = func_0202c430(t0, (u32)data_0204ad00);
    }
    return *(u32 *)(data_0204ad00 + 32);
}
