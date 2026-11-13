#include "types.h"

extern u8 data_02042a68[];
extern void func_0201bd1c(void);

void func_020067fc(void)
{
    u8 *p = *(u8 **)(data_02042a68 + 8);

    func_0201bd1c();
    *(u32 *)(p + 0xa0) = 0xffffffff;
}
