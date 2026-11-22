#include "types.h"

typedef struct T T;
struct T { u8 pad[0x118]; s32 f118; };

extern void func_0201cdb8(T *);

void func_0201f21c(T *t)
{
    if (t->f118 == 0)
        return;
    t->f118 -= 1;
    if (t->f118 != 0)
        return;
    func_0201cdb8(t);
}