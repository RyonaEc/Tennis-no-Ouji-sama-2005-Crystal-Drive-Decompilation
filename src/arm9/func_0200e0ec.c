#include "types.h"

typedef struct {
    u16 count;
    u16 slot[15];
} S_0200e0ec;

extern u32 func_02004e08(void);

void func_0200e0ec(S_0200e0ec *s)
{
    u16 i;
    u16 r;
    u16 t;

    s->count = 0;
    for (i = 0; i < 15; i++)
        s->slot[s->count++] = i;
    for (i = 0; i < 15; i++) {
        r = (u16)((s32)func_02004e08() % 15);
        t = s->slot[i];
        s->slot[i] = s->slot[r];
        s->slot[r] = t;
    }
}