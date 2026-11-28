#include "types.h"

extern u8 data_02042a68[];
extern u8 data_ov008_0204f440[];

extern u32 func_02017b60(u8 *a0, s32 a1, s32 a2);
extern void func_020263cc(void);
extern void func_ov008_0204c9b8(void);
extern void func_ov008_0204e084(void);
extern u32 func_ov008_0204e048(void);
extern u32 func_0203447c(u32 a0, u32 a1, u32 a2);
extern void func_ov008_0204ed64(void);
extern void func_ov008_0204e60c(void);
extern void func_02024a54(u32 intrBit, void *function);
extern u32 func_02024910(u32 a0);
extern u32 func_02020358(s32 a0);
extern void func_ov008_0204e51c(void);

void func_ov008_0204e658(void)
{
    u32 v = func_02017b60(*(u8 **)data_02042a68, 0x49fbc, 4);
    u8 *h;

    *(u32 *)data_ov008_0204f440 = v;
    if (v == 0)
        func_020263cc();
    h = *(u8 **)data_ov008_0204f440;
    func_ov008_0204c9b8();
    func_ov008_0204e084();
    func_ov008_0204e048();
    *(volatile u16 *)0x04000304 = *(volatile u16 *)0x04000304 & ~0x8000;
    func_0203447c((u32)h, 0, 0x40);
    *(volatile u32 *)0x04000000 = 0x08111118;
    *(volatile u16 *)0x04000050 = 0;
    *(volatile u32 *)0x04001000 = 0x00211010;
    *(volatile u16 *)0x04001050 = 0;
    func_ov008_0204ed64();
    func_ov008_0204e60c();
    (void)*(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = 0;
    func_02024a54(1, func_ov008_0204e51c);
    func_02024910(1);
    func_02020358(1);
    (void)*(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = 1;
}