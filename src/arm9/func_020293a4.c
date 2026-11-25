#include "types.h"

extern u32 func_02026350(void);
extern void func_0202514c(u8 *queue);
extern void func_02026364(u32 bak);
extern s32 func_02028af4(u8 *p_file, u32 command);
extern u32 func_02029444(u8 *p_arc);

void func_020293a4(u8 *p_file)
{
    u8 *p_arc = *(u8 **)(p_file + 8);

    while (p_file) {
        u32 bak = func_02026350();
        u32 is_sync = (*(u32 *)(p_file + 0xc) & 4) ? 1 : 0;

        if (is_sync) {
            func_0202514c(p_file + 0x18);
            break;
        } else {
            *(u32 *)(p_file + 0xc) |= 8;
        }

        func_02026364(bak);

        if (func_02028af4(p_file, *(u32 *)(p_file + 0x10)) == 6)
            break;

        p_file = (u8 *)func_02029444(p_arc);
    }
}