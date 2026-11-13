#include "types.h"

extern u8 data_02048c82[];
extern void func_020206e0(u8 *);
void func_02020694(void)
{
    *(volatile u32 *)0x04000000 &= ~0x40000000;
    func_020206e0(data_02048c82);
}
