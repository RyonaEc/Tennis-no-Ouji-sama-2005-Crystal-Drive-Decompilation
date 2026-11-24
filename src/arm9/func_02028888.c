#include "types.h"

typedef struct {
    u32 w0;
    u16 own_id;
    u16 index;
    u32 pos;
} FSDirPos;

typedef struct {
    u32 start;
    u16 index;
    u16 parent;
} FSArchiveFNT;

typedef struct {
    u8 *arc;
    u32 pos;
} FSiSyncReadParam;

extern void func_02028a38(FSiSyncReadParam *param, void *buf, u32 size);

s32 func_02028888(u8 *p_dir)
{
    u8 *p_arc = *(u8 **)(p_dir + 8);
    FSDirPos *arg = (FSDirPos *)(p_dir + 0x2c);
    FSArchiveFNT fnt_entry;
    FSiSyncReadParam param;

    param.arc = p_arc;
    param.pos = *(u32 *)(p_arc + 0x28) + arg->own_id * sizeof(fnt_entry);
    func_02028a38(&param, &fnt_entry, sizeof(fnt_entry));

    *(FSDirPos *)(p_dir + 0x1c) = *arg;

    if (arg->index == 0 && arg->pos == 0) {
        *(u16 *)(p_dir + 0x22) = fnt_entry.index;
        *(u32 *)(p_dir + 0x24) = *(u32 *)(p_arc + 0x28) + fnt_entry.start;
    }

    *(u32 *)(p_dir + 0x28) = fnt_entry.parent & 0xfff;

    return 0;
}