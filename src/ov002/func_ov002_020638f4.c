#include "types.h"

extern u8 data_ov002_020fdc30[];
extern u8 data_ov002_02102070[];

typedef struct {
    u32 v[3];
} T12;

void func_ov002_020638f4(u32 a0)
{
    u8 *b = *(u8 **)data_ov002_02102070 + 0x1744 + a0 * 0x38;
    T12 t = *(T12 *)data_ov002_020fdc30;

    *(u32 *)(b + 0x18) = t.v[*b];
}