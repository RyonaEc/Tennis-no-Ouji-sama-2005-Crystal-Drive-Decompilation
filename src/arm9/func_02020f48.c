#include "types.h"

typedef struct {
    u16 f0;
    u8 pad2[0xe];
    u16 f10;
} S_02048c74;

extern S_02048c74 data_02048c74;
extern void func_02021538(u32 arg);

void func_02020f48(u32 mode)
{
    u16 old0;
    u16 old10;

    old10 = data_02048c74.f10;
    old0 = data_02048c74.f0;
    data_02048c74.f0 = (old0 | old10) & ~mode;
    data_02048c74.f10 = mode;
    switch (mode) {
    case 0x20:
        *(u32 *)0x04000000 |= 0x80000000;
        *(u8 *)0x04000245 = 0x85;
        break;
    case 0x40:
        *(u32 *)0x04000000 |= 0x80000000;
        *(u8 *)0x04000246 = 0x85;
        break;
    case 0:
        *(u32 *)0x04000000 &= ~0x80000000;
        break;
    }
    func_02021538(data_02048c74.f0);
}