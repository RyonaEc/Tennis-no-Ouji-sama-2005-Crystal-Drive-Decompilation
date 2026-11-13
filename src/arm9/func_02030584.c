#include "types.h"

extern u32 func_02024af8(u32, u32);
extern u32 func_02026364(u32);

typedef struct {
    u32 f0x0;
    u32 f0x4;
} S_02030584;

u32 func_02030584(u32 a0, u32 a1)
{
    u32 t0, t1;
    if (((S_02030584 *)a1)->f0x0 == 0x0) {
        t0 = func_02024af8(a0, ((S_02030584 *)a1)->f0x0);
    }
    t1 = func_02026364(((S_02030584 *)a1)->f0x4);
    return t1;
}
