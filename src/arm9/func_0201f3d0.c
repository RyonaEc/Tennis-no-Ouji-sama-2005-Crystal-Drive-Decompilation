#include "types.h"

extern void func_0201f38c(u8 *a0, u32 a1, u32 a2);
extern void func_0201fc78(u8 *a0, u32 a1);

typedef struct {
    s32 f0 : 1;
    s32 f1 : 1;
    s32 f2 : 1;
    s32 f3 : 1;
    s32 rest : 28;
} Flags;

void func_0201f3d0(u8 *a0, u32 a1)
{
    s32 b = ((Flags *)(a0 + 0x108))->f1;

    if (b == 0) {
        func_0201f38c(a0, b, a1);
        return;
    }
    if (a1 == 0) {
        func_0201f38c(a0, b, a1);
        return;
    }
    func_0201fc78(a0 + 0xe0, 0);
    ((Flags *)(a0 + 0x108))->f3 = 1;
    *(u32 *)(a0 + 0x130) = 0;
}