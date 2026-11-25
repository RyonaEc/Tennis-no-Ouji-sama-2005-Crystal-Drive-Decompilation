#include "types.h"

typedef struct {
    u32 arc;
    u32 id;
} FSFileID;

extern u32 func_02029c8c(void *a0);
extern void func_0202a570(FSFileID *file_id, u8 *o);
extern u32 func_02029980(void *p_file, FSFileID file_id);
extern void func_0202a598(u8 *o);
extern u32 func_02029828(void *a0, u32 a1, u32 a2);
extern void func_0202a45c(u8 *o, void *p);
extern s32 func_020298e4(void *a0);

s32 func_0202a318(u8 *o)
{
    u8 blk[0x48];
    FSFileID id;

    func_02029c8c(blk);
    func_0202a570(&id, o);
    if (func_02029980(blk, id) == 0)
        return 0;
    func_0202a598(o);
    func_02029828(blk, *(u32 *)(o + 4), *(u32 *)(blk + 0x24) - *(u32 *)(blk + 0x20));
    func_0202a45c(o, blk);
    func_020298e4(blk);
    return 1;
}