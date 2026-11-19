#include "types.h"

extern u32 data_02042370[];
extern u32 func_02021a54(void);
extern u32 func_0202688c(u32, u32, u32, u32);
extern u32 func_020269d0(u32, u32, u32);
u32 func_02023d38(u32 a0, u32 a1, u32 a2)
{
    u32 ip = func_02021a54();
    u32 r;

    if (data_02042370[0] != 0xffffffff)
        r = func_0202688c(data_02042370[0], a0, ip + a1, a2);
    else
        r = func_020269d0(a0, ip + a1, a2);
    return r;
}
