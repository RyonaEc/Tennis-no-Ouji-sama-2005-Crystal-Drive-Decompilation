#include "types.h"

extern u8 data_02048d3c[];

void func_02025508(u8 *node)
{
    u8 *prev = 0;
    u8 *t = *(u8 **)(data_02048d3c + 0xc);

    while (t != 0 && t != node) {
        prev = t;
        t = *(u8 **)(t + 0x68);
    }
    if (prev == 0)
        *(u8 **)(data_02048d3c + 0xc) = *(u8 **)(node + 0x68);
    else
        *(u8 **)(prev + 0x68) = *(u8 **)(node + 0x68);
}