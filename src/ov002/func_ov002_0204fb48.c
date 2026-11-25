#include "types.h"

extern void func_ov002_0204da1c(u8 *a0, u16 a1);
extern u32 func_ov002_0204d488(void);
extern u32 func_ov002_02068438(void);
extern u32 func_ov002_0204db24(u32 a0);

void func_ov002_0204fb48(u32 a0, u32 a1)
{
    u8 buf[0x44];

    func_ov002_0204da1c(buf + 8, 0);
    func_ov002_0204da1c(buf + 0, 1);
    func_ov002_0204da1c(buf + 0x28, 2);
    *(u16 *)(buf + 0xa) = (u16)func_ov002_0204d488();
    *(u8 *)(buf + 0x1a) = (u8)func_ov002_02068438();
    *(u16 *)(buf + 0x30) = (u16)a0;
    *(u16 *)(buf + 0x18) = 0x7fff;
    *(u32 *)(buf + 0x20) = 0xfa0;
    *(u32 *)(buf + 0x24) = 0x120;
    *(u8 *)(buf + 0x1d) = 3;
    *(u16 *)(buf + 0x32) = (u16)a1;
    *(u8 *)(buf + 0x1b) = 0;
    *(s16 *)(buf + 0x10) = -4;
    *(s16 *)(buf + 0x12) = -4;
    *(u16 *)(buf + 0x14) = 8;
    *(u16 *)(buf + 0x16) = 8;
    *(u8 *)(buf + 0x1c) = 0;
    *(u8 *)(buf + 0x1e) = 0;
    *(u8 *)(buf + 0x1f) = 0;
    *(u16 *)(buf + 0x34) = 0;
    *(u32 *)(buf + 0x38) = 0x1000;
    *(u32 *)(buf + 0x3c) = 0x1000;
    func_ov002_0204db24((u32)buf);
    func_ov002_0204db24((u32)(buf + 8));
    func_ov002_0204db24((u32)(buf + 0x28));
}