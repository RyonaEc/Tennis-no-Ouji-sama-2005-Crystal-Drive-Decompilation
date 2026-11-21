#include "types.h"

typedef unsigned char u8;
typedef short s16;

extern u8 *data_ov002_02102070;

typedef struct {
    u8 pad[0x138];
    s16 hp;
} Unit;

void func_ov002_02058998(int i)
{
    Unit *p = (Unit *)(data_ov002_02102070 + 0x2e4 + i * 0x158);

    if (p->hp > 0)
        p->hp -= 1;
}