#include "types.h"

typedef struct {
    u32 *cur;
    u32 *next;
} W;

void func_02021c2c(W *w, u32 v, s32 flags)
{
    *w->cur = 0x2b;
    *w->next = v >> (4 - (flags == 2 ? 1 : 0));
    w->cur = w->next + 1;
    w->next = w->cur + 1;
}