#include "types.h"

typedef s32 (*FsProc)(u8 *p_file, u32 command);

extern u32 data_02041e4c[];
extern u32 func_02026350(void);
extern void func_020251c0(u32 *p);
extern void func_02026364(u32 bak);
extern void func_02028c98(u8 *p_file, s32 ret);

#define IS_SYNC(a) (((*(u32 *)((a) + 0x10) & 0x200) ? 1 : 0) != 0)

s32 func_02028af4(u8 *p_file, u32 command)
{
    s32 ret;
    u8 *p_arc = *(u8 **)(p_file + 8);
    const int bit = 1 << command;
    u32 is_sync;

    is_sync = (*(u32 *)(p_file + 0xc) & 4) ? 1 : 0;

    if (is_sync)
        *(u32 *)(p_arc + 0x10) |= 0x200;
    else
        *(u32 *)(p_arc + 0x10) |= 0x100;

    if (*(u32 *)(p_arc + 0x4c) & bit) {
        ret = (*(FsProc *)(p_arc + 0x48))(p_file, command);

        switch (ret) {
        case 0:
        case 1:
        case 4:
            *(s32 *)(p_file + 0x14) = ret;
            break;
        case 6:
            break;
        case 8:
            ret = 7;
            *(u32 *)(p_arc + 0x4c) &= ~bit;
            break;
        }
    } else {
        ret = 7;
    }

    if (ret == 7)
        ret = (*(s32 (*)(u8 *))data_02041e4c[command])(p_file);

    if (ret == 6) {
        is_sync = (*(u32 *)(p_file + 0xc) & 4) ? 1 : 0;

        if (is_sync) {
            u32 bak = func_02026350();

            while (IS_SYNC(p_arc))
                func_020251c0((u32 *)(p_arc + 0xc));
            ret = *(s32 *)(p_file + 0x14);
            func_02026364(bak);
        }
    } else {
        is_sync = (*(u32 *)(p_file + 0xc) & 4) ? 1 : 0;

        if (!is_sync) {
            *(u32 *)(p_arc + 0x10) &= ~0x100;
            func_02028c98(p_file, ret);
        } else {
            *(u32 *)(p_arc + 0x10) &= ~0x200;
            *(s32 *)(p_file + 0x14) = ret;
        }
    }

    return ret;
}