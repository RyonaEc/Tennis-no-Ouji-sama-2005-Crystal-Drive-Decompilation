#include "types.h"

typedef struct S S;
struct S { u8 *buf; u32 cap; u32 used; };

extern void func_02034494(u8 *, u32, u32);

int func_02034608(S *s, u32 src, u32 n)
{
    u32 cnt = n;

    if (s->used + n > s->cap)
        cnt = s->cap - s->used;
    func_02034494(s->buf + s->used, src, cnt);
    s->used += cnt;
    return 1;
}