#include "types.h"

typedef struct {
    u8 count;
    u8 pad1[3];
    u32 f4;
} S_0201e6c4;

extern u32 func_0201d5b8(void);
extern u32 func_0201d360(s32 idx);
extern void func_0201c63c(u32 a0, u32 a1);
extern s32 func_0201c59c(u32 a0, u32 a1, u32 a2);

s32 func_0201e6c4(u32 p)
{
    s32 i;
    S_0201e6c4 *e;
    s32 j;

    func_0201d5b8();
    for (i = 0; i < 0x20; i++) {
        e = (S_0201e6c4 *)func_0201d360(i);
        if (e == 0)
            continue;
        func_0201c63c(i, e->count);
        if (e->f4 == 0)
            continue;
        if (p == 0)
            continue;
        for (j = 0; j < e->count; j++) {
            if (func_0201c59c(i, p, e->f4) == 0)
                return 0;
        }
    }
    return 1;
}