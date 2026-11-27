#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_0203447c(u32 a0, u32 a1, u32 a2);

void func_ov002_02084db0(u8 *a0)
{
    u32 i, j;
    u8 *e = *(u8 **)data_ov002_02102070 + 0x47ac;
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *row;
    u8 *ep;

    func_0203447c((u32)e, 0, 0x21);
    row = e;

    for (i = 0; i < 4; i++) {
        e[i] = a0[i];
        ep = e + i;
        for (j = 0; j < 5; j++)
            *(u8 *)(row + j + 4) = ep[0];
        *(u8 *)(ep + 0x18) = 5;
        j = p[0xbb];
        while (j < 5) {
            *(u8 *)(row + j + 4) = 0;
            j++;
        }
        *(u8 *)(ep + 0x18) = p[0xbb];
        row += 5;
    }
    e[0x20] = 0;
}