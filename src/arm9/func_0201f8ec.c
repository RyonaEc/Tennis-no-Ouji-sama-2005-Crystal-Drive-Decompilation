#include "types.h"

extern u8 data_02048760[];
extern void func_0201f3d0(u8 *, u8 *);

void func_0201f8ec(u8 *p)
{
    s32 i;
    u8 *e = data_02048760;

    for (i = 0; i < 4; i++) {
        func_0201f3d0(e, p);
        e += 0x144;
    }
}