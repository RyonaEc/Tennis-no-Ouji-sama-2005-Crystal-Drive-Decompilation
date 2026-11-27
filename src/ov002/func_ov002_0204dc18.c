#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_0202472c(u32 a0, u32 a1, u32 a2);
extern u32 func_02022190(u32 a0, u32 a1);
extern u32 func_02022120(u32 a0);
extern u32 func_02022048(u32 a0, u32 a1, u32 a2, u32 a3);
extern u32 func_02022084(u32 a0, u32 a1, u32 a2, u32 a3);
extern void func_02021e4c(u32 a0, u32 a1, u32 a2, u32 a3);
extern void func_02021e10(u32 a0, u32 a1, u32 a2, u32 a3);

void func_ov002_0204dc18(u32 a0)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *b = p + 0x1a89a4 + a0 * 0x10018;

    func_0202472c((u32)b, (u32)(b + 0x18), 0x10000);
    func_02022190((u32)b, 0);
    func_02022120((u32)b);
    func_02022190((u32)b, 3);
    func_02022120((u32)b);
    func_02022190((u32)b, 2);
    func_02022120((u32)b);
    func_02022048((u32)b, -0x1000, 0x1000, 0);
    func_02022084((u32)b, 0x1000, 0xffffeaab, 0x1000);
    func_02021e4c((u32)b, 0, 0, 0);
    func_02021e10((u32)b, 0, 0, 0);
}