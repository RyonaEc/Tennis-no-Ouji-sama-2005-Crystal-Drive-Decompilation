#include "types.h"

extern u8 data_ov002_020fb408[];
extern u8 data_ov002_020fb40c[];
extern u8 data_ov002_020fb544[];

typedef struct {
    u8 f0x0;
    u8 pad_1[295];
    u32 f0x128;
    u16 f0x12c;
} S_ov002_02085f74;

u32 func_ov002_02085f74(u32 a0)
{
    u32 v0;
    v0 = ((S_ov002_02085f74 *)a0)->f0x0;
    ((S_ov002_02085f74 *)a0)->f0x128 = *(u32 *)((u8 *)(((u32)data_ov002_020fb408 + (*(u8 *)((u8 *)((v0 * 0x34 + (u32)data_ov002_020fb544)) + 38) * 0x38))));
    ((S_ov002_02085f74 *)a0)->f0x12c = *(u16 *)((u8 *)(((u32)data_ov002_020fb40c + (*(u8 *)((u8 *)((v0 * 0x34 + (u32)data_ov002_020fb544)) + 40) * 0x38))));
    return (a0 + 0x100);
}
