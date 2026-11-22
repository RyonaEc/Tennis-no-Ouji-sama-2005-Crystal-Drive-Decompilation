#include "types.h"

typedef struct {
    u32 f0;
    u32 f4;
} Ctx;

extern void *func_02018248(u32, u32, u32);
extern u32 func_02017770(u32 *, u32);
extern void func_020178e8(u32, void *);

void *func_0201da30(Ctx *ctx, u32 a1, u32 a2, u32 a3, u32 a4)
{
    u32 *n;
    u32 h;

    n = (u32 *)func_02018248(ctx->f0, ((a1 + 0x1f) & ~0x1f) + 0x20, 0x20);
    if (n == 0)
        return 0;
    h = func_02017770(&ctx->f4, 0);
    n[2] = a1;
    n[3] = a2;
    n[4] = a3;
    n[5] = a4;
    func_020178e8(h, n);
    return (u8 *)n + 0x20;
}