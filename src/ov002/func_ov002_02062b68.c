#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_02062b68(u32 a0)
{ u8 *b = B; u32 *r = (u32 *)b; u32 v; r += 0xa; v = r[0]; r[0] = v | 4; b[0x2c] = a0; }