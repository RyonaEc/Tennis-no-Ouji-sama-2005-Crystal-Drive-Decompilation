#include "types.h"

/* The repetition of the four pool-derived pointers in each argument list is
   required: retail re-loads the module pointer and its 0x4fec/0x4ff0/0x4ff4/
   0x4fe8 fields for the second call instead of keeping them in locals, which
   costs the extra r4-r8 push of a hoisted version (208 B vs 236 B). */

extern u8 data_ov002_02102070[];
extern void func_ov002_020892fc(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);

void func_ov002_02089210(s32 a0)
{
    func_ov002_020892fc((s32)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4fec) + (*(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4ff0) + *(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4ff4)) * 4)) * 4),
        *(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4fec) + (*(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4ff0) + *(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4ff4)) * 4)) * 4) * 0x14 + (s32)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4fe8)),
        0x68,
        8,
        0xd,
        0x06608200,
        0,
        0);
    func_ov002_020892fc((s32)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4fec) + (*(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4ff0) + *(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4ff4) + a0 * 4 + 4) * 4)) * 4),
        *(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4fec) + (*(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4ff0) + *(u16 *)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4ff4) + a0 * 4 + 4) * 4)) * 4) * 0x14 + (s32)(*(u8 **)(*(u8 **)data_ov002_02102070 + 0x4fe8)),
        0x88,
        8,
        0xd,
        0x06608200,
        0,
        0);
}
