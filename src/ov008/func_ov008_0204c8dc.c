#include "types.h"

extern u32 data_ov008_0204f440[];

extern u32 func_0202472c(u32 a0, u32 a1, u32 a2);
extern u32 func_02022190(u32 a0, u32 a1);
extern u32 func_02022120(u32 a0);
extern u32 func_02022048(u32 a0, u32 a1, u32 a2, u32 a3);
extern u32 func_02022084(u32 a0, u32 a1, u32 a2, u32 a3);
extern void func_02021e4c(u32 a0, u32 a1, u32 a2, u32 a3);
extern void func_02021e10(u32 a0, u32 a1, u32 a2, u32 a3);

void func_ov008_0204c8dc(u32 a0)
{
    u8 *p = (u8 *)data_ov008_0204f440[0] + 0x2576c + a0 * 0x10018;

    func_0202472c((u32)p, (u32)(p + 0x18), 0x10000);
    func_02022190((u32)p, 0);
    func_02022120((u32)p);
    func_02022190((u32)p, 3);
    func_02022120((u32)p);
    func_02022190((u32)p, 2);
    func_02022120((u32)p);
    func_02022048((u32)p, (u32)-0x1000, 0x1000, 0);
    func_02022084((u32)p, 0x1000, 0xffffeaab, 0x1000);
    func_02021e4c((u32)p, 0, 0, 0);
    func_02021e10((u32)p, 0, 0, 0);
}