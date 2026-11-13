#include "types.h"

extern u8 data_02048c84[];
extern void func_02020878(u8 *);
void func_02020830(void)
{
    *(volatile u32 *)0x04000000 &= ~0x80000000;
    func_02020878(data_02048c84);
}
