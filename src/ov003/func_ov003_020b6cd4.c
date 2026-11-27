#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_02025c94(u32 a0, u32 a1);
extern void func_02026a50(const void *srcp, void *destp, u32 size);
extern void func_02023eb8(u32 a0, u32 a1, u32 a2);
extern void func_ov003_020540fc(void);

void func_ov003_020b6cd4(void)
{
    u8 *p = *(u8 **)data_ov003_020eb720;

    func_02025c94((u32)(p + 0x89e8), 0x400);
    func_02026a50(p + 0x89e8, (void *)0x07000000, 0x400);
    func_02025c94((u32)(p + 0x8de8), 0x400);
    func_02023eb8((u32)((u8 *)p + 0x8de8), 0, 0x400);

    *(volatile u32 *)0x04000014 = (*(u16 *)(p + 0xadec) & 0x1ff)
        | (0x01ff0000 & (*(u16 *)(p + 0xadee) << 16));
    *(volatile u32 *)0x0400001c = (*(u16 *)(p + 0xadf4) & 0x1ff)
        | (0x01ff0000 & (*(u16 *)(p + 0xadf6) << 16));

    func_ov003_020540fc();
}