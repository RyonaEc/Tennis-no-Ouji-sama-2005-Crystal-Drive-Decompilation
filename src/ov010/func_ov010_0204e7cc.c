#include "types.h"

extern void func_02005418(void);

extern u8 data_ov010_0205e500[];
extern u8 data_02042a68[];
extern void func_020184d0(u32);
extern s32 func_02017b20(u32, u32);
extern void func_02024a54(u32, void *);

void func_ov010_0204e7cc(void)
{
    u32 v;
    u16 old;
    u16 cur;

    func_020184d0((u32)(*(u8 **)data_ov010_0205e500 + 0x3034));
    v = *(u32 *)(*(u8 **)data_ov010_0205e500 + 0x3030);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov010_0205e500 + 0x302c);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov010_0205e500 + 0x2fdc);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov010_0205e500 + 0x2fe0);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov010_0205e500 + 0x2fe4);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov010_0205e500 + 0x2fe8);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    func_02017b20(*(u32 *)data_02042a68, (u32)*(u8 **)data_ov010_0205e500);
    old = *(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = 0;
    func_02024a54(1, (void *)func_02005418);
    cur = *(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = 1;
}