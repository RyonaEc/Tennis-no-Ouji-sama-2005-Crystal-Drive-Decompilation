#include "types.h"

extern u32 data_0204abfc;
extern void func_0202cc54(u32 id);
extern u32 func_0202cc34(u32 id);

s32 func_02029ebc(u8 *p_file, u32 cmd)
{
    (void)p_file;

    switch (cmd) {
    case 9:
        func_0202cc54((u16)data_0204abfc);
        return 0;
    case 10:
        func_0202cc34((u16)data_0204abfc);
        return 0;
    case 1:
        return 4;
    default:
        return 8;
    }
}