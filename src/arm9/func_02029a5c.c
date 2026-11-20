#include "types.h"

extern void func_02029c8c(void *);
extern u32 func_02029b2c(void *, u32, u32, u32);

u32 func_02029a5c(u32 a0, u32 a1)
{
    u8 buf[0x44];
    func_02029c8c(buf);
    return func_02029b2c(buf, a1, a0, 0) != 0;
}