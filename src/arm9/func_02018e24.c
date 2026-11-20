#include "types.h"

extern void func_020192c8(u32, u32, u32);
extern void func_02019234(u32, u32, u32);
extern u32 func_02019668(u32);
extern void func_02019170(u32, u32, u32);

void func_02018e24(u8 *a0)
{
    if (*(u32 *)(a0 + 0x14) != 0)
    {
        func_020192c8(*(u32 *)a0, *(u16 *)(a0 + 4), *(u16 *)(a0 + 6));
        return;
    }
    func_02019234(*(u32 *)a0, *(u16 *)(a0 + 4), *(u16 *)(a0 + 6));
    if (func_02019668((u32)(a0 + 0xa)) == 0) return;
    func_02019170(*(u32 *)a0, *(u16 *)(a0 + 0xa), *(u16 *)(a0 + 0xc));
}