#include "types.h"

extern s32 func_020291f4(u8 *p_file, u32 command);
extern s32 func_02029838(u8 *p_file);

s32 func_02029aa0(u8 *p_file, void *dst, s32 len, u32 async)
{
    const s32 pos = *(s32 *)(p_file + 0x28);
    const s32 rest = *(s32 *)(p_file + 0x24) - pos;
    const u32 org = (u32)len;

    if (len > rest)
        len = rest;
    if (len < 0)
        len = 0;

    *(void **)(p_file + 0x2c) = dst;
    *(u32 *)(p_file + 0x30) = org;
    *(u32 *)(p_file + 0x34) = (u32)len;

    if (!async)
        *(u32 *)(p_file + 0xc) |= 4;

    func_020291f4(p_file, 0);

    if (!async) {
        if (func_02029838(p_file))
            len = *(s32 *)(p_file + 0x28) - pos;
        else
            len = -1;
    }

    return len;
}