#include "types.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef union {
    u32 raw;
    struct {
        u32 tag : 5;
        u32 err : 1;
        u32 data : 26;
    } e;
} PXIFifoMessage;

extern u32 func_02026350(void);
extern void func_02026364(u32 a);

s32 func_02027e98(s32 fifotag, u32 data, s32 err)
{
    PXIFifoMessage fifomsg;
    u32 enabled;

    fifomsg.e.tag = (u32)fifotag;
    fifomsg.e.err = (u32)err;
    fifomsg.e.data = data;

    if (*(volatile u16 *)0x04000184 & 0x4000) {
        *(volatile u16 *)0x04000184 |= 0xc000;
        return -1;
    }

    enabled = func_02026350();

    if (*(volatile u16 *)0x04000184 & 2) {
        func_02026364(enabled);
        return -2;
    }

    *(volatile u32 *)0x04000188 = fifomsg.raw;
    func_02026364(enabled);

    return 0;
}