#include "types.h"

extern u8 data_02048d3c[];

void func_02025550(u8 *a0)
{
    u8 *head = *(u8 **)(data_02048d3c + 0xc);
    u8 *cur = *(u8 **)(data_02048d3c + 0xc);
    u8 *prev = 0;

    while (cur != 0 && *(u32 *)(cur + 0x70) < *(u32 *)(a0 + 0x70)) {
        prev = cur;
        cur = *(u8 **)(cur + 0x68);
    }
    if (prev == 0) {
        *(u8 **)(a0 + 0x68) = head;
        *(u8 **)(data_02048d3c + 0xc) = a0;
    } else {
        *(u8 **)(a0 + 0x68) = *(u8 **)(prev + 0x68);
        *(u8 **)(prev + 0x68) = a0;
    }
}