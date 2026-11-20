#include "types.h"

typedef struct {
    u32 b0 : 1;
    u32 rest : 31;
} F;

#define P (*(u32 *)(data_02042a68 + 0x14))

extern u8 data_02042a68[];
extern s32 func_0201745c(void);
extern s32 func_020173a0(u32);
extern s32 func_02017380(u32);
extern s32 func_02014d34(u32);
extern void func_02026b88(s32, s32, u32);
extern void func_02026af4(s32, s32, u32);

void func_0201484c(void)
{
    u16 i;

    if (((F *)P)->b0 == 0 || func_0201745c() != 5) {
        *(u32 *)(P + 8) = 0;
        return;
    }
    {
        *(u32 *)(P + 8) = func_020173a0(*(u32 *)(P + 0x104));
        if (*(u32 *)(P + 8) != 0) return;
        for (i = 0; i < 8; i++) {
            s32 a = func_02017380(i);
            s32 b = func_02014d34(i);
            if (a != 0) func_02026b88(a, b, 0x18);
            else func_02026af4(b, 0, 0x18);
        }
    }
}