#include "types.h"

extern u8 data_02042a68[];
typedef struct { u32 b:1; } F;
extern void func_020175dc(void);
void func_02015368(void)
{
    if (((F *)*(u8 **)(data_02042a68 + 0x14))->b) return;
    func_020175dc();
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) = (*(u32 *)(*(u8 **)(data_02042a68 + 0x14)) & ~1) | 1;
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) &= ~2;
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) &= ~8;
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) &= ~0x10;
}
