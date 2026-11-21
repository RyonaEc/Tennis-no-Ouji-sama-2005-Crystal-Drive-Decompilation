#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

void func_ov002_0204c7e8(u8 *p, u16 a1)
{ u16 *q = (u16 *)p; u32 *r = (u32 *)p; q[3] = 0; q[2] = a1; r += 6; r[0] = r[0] | 2; }