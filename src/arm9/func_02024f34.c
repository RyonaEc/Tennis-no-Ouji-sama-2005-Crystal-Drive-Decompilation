#include "types.h"

extern u8 data_02048d3c[];
extern u32 func_02026350(void);
extern void func_02026364(u32);
u32 func_02024f34(u32 v)
{
    u32 state = func_02026350();
    u32 old = *(u32 *)(data_02048d3c + 0x10);
    *(u32 *)(data_02048d3c + 0x10) = v;
    func_02026364(state);
    return old;
}
