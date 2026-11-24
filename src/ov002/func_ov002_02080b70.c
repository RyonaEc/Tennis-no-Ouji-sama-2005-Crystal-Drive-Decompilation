#include "types.h"

extern u8 data_ov002_02102070[];

typedef unsigned char u8;
typedef signed int s32;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 mode;
    u8 pad[0x111];
    s32 val;
    u8 pad2[0x40];
} Ent;

typedef struct {
    u8 pad[0x14];
    s32 f[2];
} Out;

void func_ov002_02080b70(void)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    int n[2] = {0, 0};
    s32 v[2][2] = {{0, 0}, {0, 0}};
    Out *o = (Out *)(base + 0x3654);
    Ent *e = (Ent *)(base + 0x2e4);
    int i;

    for (i = 0; i < 4; i++, e++) {
        if (e->unk0 == 0)
            continue;
        if (e->mode < 2) {
            v[0][n[0]] = e->val;
            n[0]++;
        } else {
            v[1][n[1]] = e->val;
            n[1]++;
        }
    }

    for (i = 0; i < 2; i++) {
        if (n[i] == 1)
            o->f[i] = v[i][0];
        else
            o->f[i] = (v[i][0] + v[i][1]) / 2;
    }

    if (base[0x8d] == 1) {
        o->f[0] *= 2;
        o->f[1] *= 2;
    }
}