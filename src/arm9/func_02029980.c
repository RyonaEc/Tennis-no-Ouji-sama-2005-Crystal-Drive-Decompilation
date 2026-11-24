#include "types.h"

typedef struct {
    u32 arc;
    u32 id;
} FSFileID;

extern s32 func_020291f4(u8 *p_file, u32 command);

u32 func_02029980(u8 *p_file, FSFileID file_id)
{
    if (!file_id.arc)
        return 0;

    *(u32 *)(p_file + 8) = file_id.arc;
    *(FSFileID *)(p_file + 0x2c) = file_id;

    if (!func_020291f4(p_file, 6))
        return 0;

    *(u32 *)(p_file + 0xc) |= 0x10;
    *(u32 *)(p_file + 0xc) &= ~0x20;

    return 1;
}