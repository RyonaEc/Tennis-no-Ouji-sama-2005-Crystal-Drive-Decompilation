#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_02025c94(u32 a0, u32 a1);
extern void func_02026a50(u32 srcp, u32 destp, u32 size);
extern void func_02023eb8(u32 a0, u32 a1, u32 a2);
extern void func_ov003_020540fc(void);

void func_ov003_020dabfc(void)
{
    u8 *p = *(u8 **)data_ov003_020eb720;

    if ((u16)(*(u16 *)(p + 0x86c2) + 0xffac) <= 1) {
        func_02025c94((u32)((u8 *)p + 0x89e8), 0x400);
        func_02026a50((u32)p + 0x89e8, 0x07000000, 0x400);
        func_02025c94((u32)(p + 0x8de8), 0x400);
        func_02023eb8(0x8de8 + (u32)p, 0, 0x400);
        *(u32 *)0x04000014 = (*(u16 *)(p + 0xadec) & 0x1ff) |
                             ((*(u16 *)(p + 0xadee) << 16) & 0x01ff0000);
        *(u32 *)0x0400001c = 0;
    }
    func_ov003_020540fc();
}