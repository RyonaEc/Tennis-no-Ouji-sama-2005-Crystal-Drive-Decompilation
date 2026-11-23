#include "types.h"

extern u8 data_ov006_02077e40[];
extern u8 data_02042a68[];
extern void func_020184d0(u32);
extern s32 func_02017b20(u32, u32);
extern void func_020126bc(u32);
extern void func_02024a54(u32, void *);

void func_ov006_0204d0fc(void)
{
    u32 v;
    u16 old;
    u16 cur;

    func_020184d0((u32)(*(u8 **)data_ov006_02077e40 + 0x21f0));
    v = *(u32 *)(*(u8 **)data_ov006_02077e40 + 0x21e8);
    if (v != 0) {
        func_02017b20(*(u32 *)data_02042a68, v);
        func_020126bc(0);
    }
    v = *(u32 *)(*(u8 **)data_ov006_02077e40 + 0x21ec);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov006_02077e40 + 0x21d8);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov006_02077e40 + 0x21dc);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov006_02077e40 + 0x21e0);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = *(u32 *)(*(u8 **)data_ov006_02077e40 + 0x21e4);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    v = (u32)(*(u8 **)data_ov006_02077e40);
    if (v != 0)
        func_02017b20(*(u32 *)data_02042a68, v);
    old = *(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = 0;
    func_02024a54(1, (void *)0x02005418);
    cur = *(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = 1;
}