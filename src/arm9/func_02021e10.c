#include "types.h"

extern void func_0202230c(u32, u32, u32);
extern void func_020221b8(u32, u32);
void func_02021e10(u32 a0, u32 a1, u32 a2, u32 a3)
{
    func_0202230c(a0, 0x31, (a1 | (a2 << 16)) | ((a3 ? 1 : 0) << 15));
    func_020221b8(a0, 1);
}
