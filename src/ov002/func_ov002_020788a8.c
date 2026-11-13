#include "types.h"

extern void func_ov002_0205958c(u8 *);
extern void func_ov002_02088c4c(u32);
extern void func_ov002_0207b35c(u32);
void func_ov002_020788a8(u8 *a0)
{
    *(u8 *)(a0 + 0xe4) = 0;
    *(u8 *)(a0 + 0xe5) = 0;
    func_ov002_0205958c(a0);
    func_ov002_02088c4c((u32)*(u8 *)(a0 + 2) >> 1);
    func_ov002_0207b35c((u32)*(u8 *)(a0 + 2) >> 1);
}
