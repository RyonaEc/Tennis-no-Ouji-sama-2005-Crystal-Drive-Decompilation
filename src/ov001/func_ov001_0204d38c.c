#include "types.h"

extern u8 data_ov001_02051cc0[];
extern u8 data_02042a68[];
extern void func_020184d0(u32);
extern s32 func_02017b20(u32, u32);
extern void func_02024a54(u32, void *);
extern void func_020248c8(u32);
extern void func_02020388(u32);

void func_ov001_0204d38c(void)
{
    u16 old;
    u16 cur;

    func_020184d0((u32)(*(u8 **)data_ov001_02051cc0 + 0x1750));
    func_02017b20(*(u32 *)data_02042a68, *(u32 *)(*(u8 **)data_ov001_02051cc0 + 0x174c));
    func_02017b20(*(u32 *)data_02042a68, (u32)*(u8 **)data_ov001_02051cc0);
    old = *(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = 0;
    func_02024a54(1, (void *)0x02005418);
    func_020248c8(2);
    func_02020388(0);
    cur = *(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = 1;
}