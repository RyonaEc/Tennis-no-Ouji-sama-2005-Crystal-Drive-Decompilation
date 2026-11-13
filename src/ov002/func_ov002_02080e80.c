#include "types.h"

extern u32 func_ov002_02081a90(void);
extern u32 func_ov002_02081a10(u32);
extern u32 func_ov002_020818d4(u32);
extern u32 func_ov002_020817b4(u32);
extern u32 func_ov002_02081670(u32);
extern u32 func_ov002_020815b0(u32);

u32 func_ov002_02080e80(void)
{
    u32 t0 = func_ov002_02081a90();
    u32 t1 = func_ov002_02081a10(t0);
    u32 t2 = func_ov002_020818d4(t1);
    u32 t3 = func_ov002_020817b4(t2);
    u32 t4 = func_ov002_02081670(t3);
    u32 t5 = func_ov002_020815b0(t4);
    return t5;
}
