#include "types.h"

extern s32 func_0203aec4(u64, u64);
extern u64 func_0203aeb8(u64, u64);

typedef struct {
    u8 unk0;
    u8 pad1;
    u16 len;
    u8 count;
    u8 digits[32];
} Str;

void func_02037590(Str *s, u64 v)
{
    u32 i;
    u8 *p;
    u8 *q;
    u8 *e;
    u8 t;

    s->unk0 = 0;
    s->count = 0;
    if (v == 0)
        goto done;
    do {
        i = s->count;
        s->count += 1;
        s->digits[i] = func_0203aec4(v, 10);
        v = func_0203aeb8(v, 10);
    } while (v != 0);
done:
    p = s->digits;
    e = p + s->count;
    q = e - 1;
    while (p < q) {
        t = *p;
        *p = *q;
        *q = t;
        p++;
        q--;
    }
    s->len = s->count - 1;
}