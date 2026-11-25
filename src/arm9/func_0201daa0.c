#include "types.h"

extern s32 func_02017770(u8 *a, u8 *b);
extern void func_0201779c(u8 *node, u8 *base);
extern void func_02018208(u32 a0, u32 a1);
extern u32 func_0201d83c(void);
extern u32 func_0201d860(u8 *a0);

u32 func_0201daa0(u8 *a0)
{
    u8 *node;
    u8 *child;

    node = (u8 *)func_02017770(a0 + 4, 0);
    while (node != 0) {
        child = (u8 *)func_02017770(node, 0);
        while (child != 0) {
            if (*(u32 *)(child + 0xc) != 0)
                ((void (*)(u8 *, u32, u32, u32))*(u32 *)(child + 0xc))(child + 0x20,
                        *(u32 *)(child + 8), *(u32 *)(child + 0x10), *(u32 *)(child + 0x14));
            child = (u8 *)func_02017770(node, child);
        }
        func_0201779c(a0 + 4, node);
        node = (u8 *)func_02017770(a0 + 4, 0);
    }
    func_02018208(*(u32 *)a0, 3);
    func_0201d83c();
    return func_0201d860(a0);
}