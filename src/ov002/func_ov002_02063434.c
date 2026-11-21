#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

extern u32 func_ov002_02062dbc(u32);

#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern u32 func_ov002_02062dbc(u32);
void func_ov002_02063434(u32 a0)
{ u8 *p = B + 0x1744 + a0 * 0x38;
  if (func_ov002_02062dbc(8) == 0) *(u32 *)(p + 0x24) |= 0x2000000;
  else *(u32 *)(p + 0x24) &= ~0x2000000; }