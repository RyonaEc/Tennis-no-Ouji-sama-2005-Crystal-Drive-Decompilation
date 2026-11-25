#include "types.h"

typedef struct {
    u8 pad0[0x44];
    u32 f44;
    u16 f48;
    u16 f4a;
} S_02019cac;

extern u8 *data_0204662c;
extern u32 func_02019754(u8 *p);
extern u32 func_0201969c(u8 *p, s32 i);

void func_02019cac(u32 a0, u8 *p)
{
    u32 *q;
    s32 idx;
    u32 t;

    if (((S_02019cac *)data_0204662c)->f44 & 1) {
        q = (u32 *)(p + 4);
        *q = (*q & ~0xc00) | ((((S_02019cac *)data_0204662c)->f48 & 3) << 10);
    }
    t = func_02019754(data_0204662c);
    if (t != 0) {
        idx = (s32)(((u32)(*(u16 *)(p + 4) & 0xf000) << 4) >> 16);
        t = func_0201969c((u8 *)t, idx);
        q = (u32 *)(p + 4);
        *q = (*q & ~0xf000) | ((t & 0xf) << 12);
    }
    if (((S_02019cac *)data_0204662c)->f44 & 2) {
        q = (u32 *)(p + 4);
        *q = (*q & ~0xf000) | ((((S_02019cac *)data_0204662c)->f4a & 0xf) << 12);
    }
}