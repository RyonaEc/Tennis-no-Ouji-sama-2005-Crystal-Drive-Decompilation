#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02062dbc(u32);
extern void func_ov002_02062b8c(void);
extern void func_ov002_02061ed0(void);
void func_ov002_02062e38(void)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    func_ov002_02062dbc(0x100);
    if (*(u16 *)(b + 4) != 0) return;
    func_ov002_02062b8c();
    if (*(u32 *)(b + 0x28) & 0x1b) return;
    func_ov002_02061ed0();
}
