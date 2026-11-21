#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_02062b20(u32 a0)
{ u8 *p = (u8 *)B; u32 *r = (u32 *)(p + 0x28); u32 v;
  *(u16 *)(p + 0x30) = 0; *(u16 *)(p + 0x32) = a0; v = r[0]; r[0] = v | 8; }