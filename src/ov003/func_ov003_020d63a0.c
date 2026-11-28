#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_020269d0(u32 a0, u32 a1, u32 a2);
extern void func_02025c94(u32 a0, u32 a1);
extern void func_02026a50(const void *a0, u32 a1, u32 a2);
extern void func_02023eb8(const void *a0, u32 a1, u32 a2);
extern void func_ov003_020d59f0(u32 a0, u32 a1);
extern void func_ov003_020540fc(void);

void func_ov003_020d63a0(void)
{
    u8 *b = *(u8 **)data_ov003_020eb720;
    u8 *p = b + 0xb8dc;
    u16 v;

    v = 0;
    func_020269d0((u32)&v, 0x05000400, 2);
    if ((u16)(*(u16 *)(b + 0x86c2) + 0xffaf) <= 1) {
        func_02025c94((u32)(b + 0x89e8), 0x400);
        func_02026a50(b + 0x89e8, 0x07000000, 0x400);
        func_02025c94((u32)(b + 0x8de8), 0x400);
        func_02023eb8(b + 0x8de8, 0, 0x400);
        func_ov003_020d59f0(*(u16 *)(p + 8), *(u16 *)(p + 4));
        *(u32 *)0x04000014 = (*(u16 *)(b + 0xadec) & 0x1ff) | ((*(u16 *)(b + 0xadee) << 16) & 0x01ff0000);
        *(u32 *)0x0400001c = 0;
    }
    func_ov003_020540fc();
}