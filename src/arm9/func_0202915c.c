#include "types.h"

extern void func_02028c98(u8 *p_file, u32 ret);
extern u32 func_02029444(u8 *p_arc);
extern void func_020293a4(u8 *p_file);
extern u32 func_02026350(void);
extern void func_0202514c(u8 *queue);
extern void func_02026364(u32 bak);

void func_0202915c(u8 *p_arc, u32 ret)
{
    u32 is_async = (*(u32 *)(p_arc + 0x10) & 0x100) ? 1 : 0;

    if (is_async) {
        u8 *p_file = *(u8 **)(p_arc + 0x18);

        *(u32 *)(p_arc + 0x10) &= ~0x100;
        func_02028c98(p_file, ret);

        p_file = (u8 *)func_02029444(p_arc);
        if (p_file)
            func_020293a4(p_file);
    } else {
        u8 *p_file = *(u8 **)(p_arc + 0x18);
        u32 bak = func_02026350();

        *(u32 *)(p_file + 0x14) = ret;
        *(u32 *)(p_arc + 0x10) &= ~0x200;
        func_0202514c(p_arc + 0xc);
        func_02026364(bak);
    }
}