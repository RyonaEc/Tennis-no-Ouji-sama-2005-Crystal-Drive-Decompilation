#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern u8 *func_ov002_020848a0(void);
extern void func_ov002_02060c88(void);
extern void func_ov002_0204f624(void);
extern void func_ov002_0204d464(void);
extern void func_ov002_02057e0c(void);
extern void func_ov002_02067acc(void);
extern void func_ov002_02078424(void);
extern void func_ov002_02081dcc(void);
extern void func_ov002_0204df58(u8 *);
void func_ov002_02063168(void)
{
    u8 *b = B;
    u8 *t = func_ov002_020848a0();
    u16 i;
    for (i = 0; i < 0x80; i++)
    {
        *(u32 *)(t + i * 8) = 0x200;
        *(u16 *)(t + 6 + i * 8) = 0;
    }
    *(u32 *)(b + 0x28) = 0;
    *(u16 *)(b + 0x2e) = 0;
    *(u16 *)(b + 0x30) = 0;
    *(u16 *)(b + 0x32) = 0;
    func_ov002_02060c88();
    *(u32 *)(b + 0x38) = 0;
    func_ov002_0204f624();
    func_ov002_0204df58(t);
    func_ov002_0204d464();
    func_ov002_02057e0c();
    func_ov002_02067acc();
    func_ov002_02078424();
    func_ov002_02081dcc();
}
