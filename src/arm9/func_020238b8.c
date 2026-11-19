#include "types.h"

extern u32 data_02042370[];
extern u32 func_020216a0(void);
extern u32 func_020268e0(u32, u32, u32, u32);
extern u32 func_020269ec(u32, u32, u32);
u32 func_020238b8(u32 a0, u32 a1, u32 a2)
{
    u32 ip = func_020216a0();
    u32 r;

    if (data_02042370[0] != 0xffffffff)
        r = func_020268e0(data_02042370[0], a0, ip + a1, a2);
    else
        r = func_020269ec(a0, ip + a1, a2);
    return r;
}
