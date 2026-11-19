#include "types.h"

extern u8 data_ov009_02054f84[];
extern void func_02020d44(u32 a0);
extern void func_02020c50(u32 a0);
extern void func_ov009_0204deec(void);
extern void func_020245f4(void);
extern void func_02024428(void);
extern void func_020243e0(void);
extern void func_02024314(void);

int func_ov009_02051154(u32 a0)
{
    u8 *p = *(u8 **)data_ov009_02054f84;

    if (a0 == 0) {
        func_02020d44(1);
        func_02020c50(0x40);
        if (*(u16 *)(p + 0x350) < 0x1f4)
            func_ov009_0204deec();
        func_020245f4();
        func_02024428();
        func_020243e0();
        func_02024314();
        *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 2;
    }
    return 0;
}