#include "types.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

extern void func_020265e0(u8 *mac);

u16 func_0202e674(void)
{
    u8 mac[6];
    u16 ret;
    s32 i;

    func_020265e0(mac);
    for (i = 0, ret = 0; i < 6; i++) {
        ret += mac[i];
    }
    ret += *(u32 *)0x027ffc3c;
    ret *= 7;
    return (u16)(200 + (ret % 20));
}