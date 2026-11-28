#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_ov003_02052100(u16 *p, u32 a1, u32 a2);
extern void func_02021b14(u32 *p, u32 a, u32 b, u32 c, u32 d);

u32 func_ov003_0206c308(void)
{
    u8 *base = *(u8 **)data_ov003_020eb720;
    u16 i;

    if (*(u16 *)(base + 0x86ce) == 1 && *(u16 *)(base + 0x8736) > 6) {
        for (i = 0; i < 4; i++)
            func_ov003_02052100(*(u16 **)(base + i * 0x1c + 0x9ff8), 0, 0xc0);
    }
    func_02021b14((u32 *)0x04001050, 0xe, 0x24,
                  *(u16 *)(base + 0x897e), *(u16 *)(base + 0x8986));
    if (*(u16 *)(base + 0x897e) != 0)
        *(u16 *)(base + 0x897e) -= 1;
    if (*(u16 *)(base + 0x8986) < 0x10)
        *(u16 *)(base + 0x8986) += 1;
    return 0;
}