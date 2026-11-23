#include "types.h"

extern u8 data_ov004_020690a8[];
extern u8 data_02042a68[];
extern void func_ov004_0205a7b4(void);
extern void func_ov004_02060868(void);
extern s32 func_ov004_02053510(u8 *);
extern void func_ov004_02053518(u8 *);
extern u32 func_ov004_020528c0(u8 *);
extern void func_ov004_020528c8(u8 *);
extern u8 *func_ov004_0204e874(s32);
extern s32 func_02017b20(u32, u32);

void func_ov004_0206036c(void)
{
    u8 *g = *(u8 **)data_ov004_020690a8;
    s32 i;
    u8 *p1;
    u8 *p2;
    u8 *q;

    func_ov004_0205a7b4();
    func_ov004_02060868();
    q = g + 0xf0;
    if (func_ov004_02053510(q) != 0)
        func_ov004_02053518(q);
    q = g + 0x178;
    if (func_ov004_02053510(q) != 0)
        func_ov004_02053518(q);
    q = g + 0x200;
    if (func_ov004_02053510(q) != 0)
        func_ov004_02053518(q);
    q = g + 0x310;
    if (func_ov004_02053510(q) != 0)
        func_ov004_02053518(q);
    q = g + 0x398;
    if (func_ov004_02053510(q) != 0)
        func_ov004_02053518(q);
    q = g + 0x420;
    if (func_ov004_02053510(q) != 0)
        func_ov004_02053518(q);
    q = g + 0x26c8;
    if (func_ov004_020528c0(q) != 0)
        func_ov004_020528c8(q);
    q = g + 0x2774;
    if (func_ov004_020528c0(q) != 0)
        func_ov004_020528c8(q);
    q = g + 0x30dc;
    if (func_ov004_020528c0(q) != 0)
        func_ov004_020528c8(q);
    q = g + 0x3188;
    if (func_ov004_020528c0(q) != 0)
        func_ov004_020528c8(q);
    p1 = g + 0x2f84;
    p2 = g + 0x3234;
    for (i = 0; i < 2; i++) {
        if (func_ov004_020528c0(p1) != 0)
            func_ov004_020528c8(p1);
        if (func_ov004_020528c0(p2) != 0)
            func_ov004_020528c8(p2);
        p1 += 0xac;
        p2 += 0xac;
    }
    q = g + 0x2820;
    for (i = 0; i < 8; i++) {
        if (func_ov004_020528c0(q) != 0)
            func_ov004_020528c8(q);
        q += 0xac;
    }
    q = g + 0x2d80;
    for (i = 0; i < 3; i++) {
        if (func_ov004_020528c0(q) != 0)
            func_ov004_020528c8(q);
        q += 0xac;
    }
    for (i = 0; i < 0x30; i++) {
        q = func_ov004_0204e874(i);
        if (q == 0)
            continue;
        if (func_ov004_020528c0(q) != 0)
            func_ov004_020528c8(q);
    }
    if (*(u32 *)(g + 0xa8) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xa8));
    if (*(u32 *)(g + 0xac) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xac));
    if (*(u32 *)(g + 0xb0) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xb0));
    if (*(u32 *)(g + 0xb4) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xb4));
    if (*(u32 *)(g + 0xbc) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xbc));
    if (*(u32 *)(g + 0xc0) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xc0));
    if (*(u32 *)(g + 0xc4) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xc4));
    if (*(u32 *)(g + 0xc8) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xc8));
    if (*(u32 *)(g + 0xb8) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xb8));
    if (*(u32 *)(g + 0xd0) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xd0));
    if (*(u32 *)(g + 0xcc) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xcc));
    if (*(u32 *)(g + 0xd4) != 0)
        func_02017b20(*(u32 *)data_02042a68, *(u32 *)(g + 0xd4));
    *(u32 *)(g + 0xa8) = 0;
    *(u32 *)(g + 0xbc) = 0;
    *(u32 *)(g + 0xc0) = 0;
    *(u32 *)(g + 0xc4) = 0;
    *(u32 *)(g + 0xc8) = 0;
    *(u32 *)(g + 0xcc) = 0;
    *(u32 *)(g + 0xd4) = 0;
    *(u32 *)(g + 0xb4) = 0;
    *(u32 *)(g + 0xb0) = 0;
    *(u32 *)(g + 0xac) = 0;
    *(u32 *)(g + 0xb8) = 0;
    *(u32 *)(g + 0xd0) = 0;
}