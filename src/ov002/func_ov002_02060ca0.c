#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_02060ca0(u32 a0)
{ u8 *p = (u8 *)B; u8 *q = p + 0x35; u8 v; v = q[0]; q[0] = v | a0; p[0x34] = 0; }