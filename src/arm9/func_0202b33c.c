#include "types.h"

typedef struct { u16 a, b, c, d; } T;
extern u8 data_0204ac68[];
extern void func_0202aea8(u8 *, u8 *);
u32 func_0202b33c(u8 *a0)
{
    T t;
    if ((*(u16 *)(data_0204ac68 + 0x34) & 1) != 0) return 1;
    t = *(T *)(data_0204ac68 + 4);
    *(T *)a0 = t;
    func_0202aea8(a0, a0);
    return 0;
}