#include "types.h"

extern s32 func_0202db70(void *);
extern void func_0202daec(void);
extern u8 data_0204b4c0[];

void func_0202d704(void)
{
    u8 *p = data_0204b4c0;

    if (func_0202db70(p) != 0)
        ((void (*)(u8 *))*(u32 *)p)(p);
    func_0202daec();
}