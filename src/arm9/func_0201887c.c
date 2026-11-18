#include "types.h"

extern void func_02025c94(u32, u32);
extern u32 data_0203d6fc[];

typedef struct {
    u32 idx;
    u32 a;
    u32 b;
    u32 c;
} T;

void func_0201887c(T *t)
{
    void (*fn)(u32, u32, u32) = (void (*)(u32, u32, u32))data_0203d6fc[t->idx];

    func_02025c94(t->a, t->c);
    fn(t->a, t->b, t->c);
}