#include "types.h"

extern u8 data_ov003_020eb720[];
extern u8 data_ov003_020eb724[];
extern u32 data_ov003_020df7a8[];
extern u32 func_02020d44(u32 a0);
extern u32 func_02020c50(u32 a0);
extern u32 func_020245f4(void);
extern void func_02026a04(u32 a0, u32 a1, u32 a2);
extern void func_02025c94(void *a0, u32 a1);
extern u32 func_02024498(void *a0, u32 a1, u32 a2);
extern u32 func_02024428(void);
extern void func_020243e0(void);
extern void func_0202436c(void *a0, u32 a1, u32 a2);
extern u32 func_02024314(void);
extern void func_ov003_020542e0(u32 a0, void *a1);

u32 func_ov003_02097bb0(u32 a0)
{
    u8 *b1 = *(u8 **)data_ov003_020eb720;
    u8 *b2 = *(u8 **)data_ov003_020eb724;

    if (a0 == 0) {
        volatile u32 z;
        u32 n1;
        u32 off1;
        u32 n2;
        u32 off2;
        u8 *p1;
        u8 *p2;

        func_02020d44(1);
        func_02020c50(0x40);
        func_020245f4();
        z = 0;
        func_02026a04(z, 0x06800000, 0x2000);
        n1 = data_ov003_020df7a8[2];
        off1 = data_ov003_020df7a8[0];
        p1 = b2 + off1;
        func_02025c94(p1, n1);
        func_02024498(p1, 0x2000, n1);
        *(u32 *)(b1 + 0xb6dc) = n1;
        func_02024428();
        func_020243e0();
        n2 = data_ov003_020df7a8[3];
        off2 = data_ov003_020df7a8[1];
        p2 = b2 + off2;
        func_02025c94(p2, n2);
        func_0202436c(p2, 0x20, n2);
        *(u32 *)(b1 + 0xb7dc) = n2;
        func_02024314();
        *(u16 *)0x04000008 = (*(u16 *)0x04000008 & ~3) | 2;
    } else {
        func_ov003_020542e0(7, b1 + 0xc8dc);
    }
    return 0;
}