#include "types.h"

extern u8 data_02048c84[];
extern void func_020206e0(u8 *);
void func_02020670(void)
{
    *(volatile u32 *)0x04000000 &= ~0x80000000;
    func_020206e0(data_02048c84);
}
