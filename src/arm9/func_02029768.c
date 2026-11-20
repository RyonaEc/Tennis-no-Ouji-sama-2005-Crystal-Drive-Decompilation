#include "types.h"

extern void func_02029c8c(void *);
extern u32 func_02029b2c(void *, u32, u32, void *);
extern u8 data_0204abec[];

typedef struct
{
    u32 a;
    u32 b;
    u32 c;
} T;

u32 func_02029768(u32 a0)
{
    T t;
    u8 buf[0x44];

    func_02029c8c(buf);
    if (func_02029b2c(buf, a0, 0, &t) == 0) return 0;
    *(T *)data_0204abec = t;
    return 1;
}