#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_ov002_02084cec(u8);
extern void func_ov002_02085940(u8);
extern void func_ov002_02080b70(void);
extern void func_ov002_02084ff4(void);
void func_ov002_020858e4(void)
{
    u8 *b = B;
    u8 *p = b + 0x2e4;
    s32 i;
    for (i = 0; i < 4; i++)
    {
        if (*(u8 *)p != 0)
        {
            func_ov002_02084cec(i);
            func_ov002_02085940(i);
        }
        p += 0x158;
    }
    func_ov002_02080b70();
    func_ov002_02084ff4();
}
