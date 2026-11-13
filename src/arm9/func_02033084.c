#include "types.h"

extern void func_02024af8(u32);
extern void func_02033bd0(u32);
extern void func_02033c1c(u32);
void func_02033084(u32 f)
{
    if (f & 0x20) func_02024af8(0x70);
    func_02033bd0(f & 2);
    func_02033c1c(f & 1);
}
