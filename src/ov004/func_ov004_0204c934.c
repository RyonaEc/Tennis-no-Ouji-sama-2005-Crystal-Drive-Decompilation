#include "types.h"

extern void func_02005418(void);

extern u8 data_ov004_020690a0[];
extern u8 data_02042a68[];
extern void func_ov004_0204eb08(void);
extern s32 func_02017b20(u32, u32);
extern u32 func_02017af8(u8 *p);
extern void func_02021af0(u8 *a0, u16 a1, s32 a2);
extern void func_02024a54(u32, void *);
extern u32 func_02024910(u32);
extern u32 func_02020358(s32);

void func_ov004_0204c934(void)
{
    u16 old;
    u16 cur;

    func_ov004_0204eb08();
    if (*(u32 *)data_ov004_020690a0 != 0) {
        old = *(volatile u16 *)0x04000208;
        *(volatile u16 *)0x04000208 = 0;
        func_02024a54(1, (void *)func_02005418);
        func_02024910(1);
        func_02020358(1);
        cur = *(volatile u16 *)0x04000208;
        *(volatile u16 *)0x04000208 = 1;
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)data_ov004_020690a0);
        *(u32 *)data_ov004_020690a0 = 0;
    }
    func_02021af0((u8 *)0x04000050, 0x3f, -0x10);
    func_02021af0((u8 *)0x04001050, 0x3f, -0x10);
    func_02017af8(*(u8 **)data_02042a68);
}