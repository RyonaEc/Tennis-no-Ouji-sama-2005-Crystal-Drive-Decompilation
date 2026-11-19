#include "types.h"

extern u32 func_02033c0c(void);
extern u32 func_02033bc0(void);
extern u32 func_02033c98(void);
extern void func_02033ca0(u32);

void func_02033a84(u32 *p)
{
    p[0] = func_02033c0c();
    p[1] = func_02033bc0();
    p[2] = func_02033c98();
    func_02033ca0(0x02600029);
}