#include "types.h"

extern u32 data_0204abf8;
extern void func_02029d94(s32 a);

void func_02029cc4(s32 a)
{
    if (data_0204abf8 != 0)
        return;
    data_0204abf8 = 1;
    func_02029d94(a);
}