#include "types.h"

typedef struct {
    u32 *cur;
    u32 *next;
} W;

void func_02021bd8(W *w, u32 v, u32 t, s32 flag)
{
    *w->cur = 0x30;
    *w->next = v | (t << 16) | ((flag ? 1 : 0) << 15);
    w->cur = w->next + 1;
    w->next = w->cur + 1;
}