#include "types.h"

typedef struct {
    s16 arr[16];
    u32 unk20;
} Ent02027a64;

typedef struct {
    u32 unk0;
    u32 unk4;
    u16 unk8;
    u16 unkA;
    u8 pad[0x14];
    Ent02027a64 ents[16];
    s16 tail[16];
} Unk02027a64;

extern void func_02025c94(void *p, u32 size);

void func_02027a64(Unk02027a64 *p)
{
    s32 i, j;

    p->unk4 = 0;
    p->unk8 = 0;
    p->unkA = 0;
    p->unk0 = 0;
    for (i = 0; i < 16; i++) {
        p->ents[i].unk20 = 0;
        for (j = 0; j < 16; j++)
            p->ents[i].arr[j] = -1;
    }
    for (i = 0; i < 16; i++)
        p->tail[i] = -1;
    func_02025c94(p, 0x280);
}