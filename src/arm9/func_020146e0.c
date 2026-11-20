#include "types.h"

typedef struct {
    u32 b0 : 1;
    u32 b1 : 1;
    u32 rest : 30;
} F;

typedef struct {
    u16 a, b;
} Q;

extern u8 data_02042a68[];
extern void func_ov002_02062d84(u8 *, u8 *);

void func_020146e0(void)
{
    F *f = (F *)*(u32 *)(data_02042a68 + 0x14);
    u8 *p;

    if (f->b0 == 0) return;
    if (f->b1 == 0) return;
    p = *(u8 **)((u8 *)f + 0x104);
    *(u16 *)(p + 0x14) = *(u16 *)(data_02042a68 + 0x30);
    *(u16 *)(p + 0x16) = *(u16 *)(data_02042a68 + 0x32);
    if (*(u16 *)(data_02042a68 + 0x26) != 1) return;
    *(Q *)(p + 0xc) = *(Q *)(data_02042a68 + 0x44);
    *(Q *)(p + 0x10) = *(Q *)(data_02042a68 + 0x48);
    func_ov002_02062d84(p + 8, p + 7);
}