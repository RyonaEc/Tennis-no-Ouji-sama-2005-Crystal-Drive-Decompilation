#include "types.h"

typedef struct
{
    u8 pad0[8];
    s32 a;
    s32 b;
} S;

void func_0201fc1c(S *s)
{
    if (s->a < s->b)
    {
        s->a = s->a + 1;
    }
}