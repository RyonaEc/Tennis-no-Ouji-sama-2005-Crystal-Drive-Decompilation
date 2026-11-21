#include "types.h"

extern void func_ov005_02051aac(void);
extern void func_ov005_02056908(s32 a0, s32 a1);
extern void func_ov005_020569a4(u32 a0);
extern void func_ov005_020569c0(s32 a0, s32 a1);

void func_ov005_02056820(void)
{
    func_ov005_02051aac();
    *(u32 *)0x040004a8 = 0x6e400000;
    *(u32 *)0x040004ac = 0;
    *(u32 *)0x040004a4 = 0x1f00c0;
    *(u32 *)0x04000500 = 1;
    func_ov005_02056908(1, 0);
    func_ov005_020569a4(0);
    func_ov005_020569c0(2, 0);
    func_ov005_02056908(0, 0);
    func_ov005_020569a4(0);
    func_ov005_020569c0(0, 0);
    func_ov005_02056908(2, 0);
    func_ov005_020569a4(0);
    func_ov005_020569c0(4, 0);
    func_ov005_02056908(3, 0);
    func_ov005_020569a4(0);
    func_ov005_020569c0(6, 0);
    *(u32 *)0x04000504 = 0;
}