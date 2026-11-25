#include "types.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

extern u32 data_0204acb0;
extern s32 func_0202bf38(s32 a);
extern u32 func_0202bf74(u32 a, u32 b, u32 c);

s32 func_0202ba54(s32 sw, s32 led, s32 skip, s32 isSync)
{
    switch (sw) {
    case 1:
        if (!skip && *(u32 *)0x027ffc3c - data_0204acb0 <= 7)
            return 0;

        if (led != 0) {
            if (isSync)
                func_0202bf38(led);
            else
                func_0202bf74(led, 0, 0);
        }

        *(volatile u16 *)0x04000304 |= 1;
        break;

    case 0:
        *(volatile u16 *)0x04000304 &= ~1;

        data_0204acb0 = *(u32 *)0x027ffc3c;

        if (led != 0) {
            if (isSync)
                func_0202bf38(led);
            else
                func_0202bf74(led, 0, 0);
        }
        break;
    }

    return 1;
}