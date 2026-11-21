#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_020781b4(void)
{ u32 *p = (u32 *)B; u32 v; p += 0x79b; v = p[0]; p[0] = v & ~1; }