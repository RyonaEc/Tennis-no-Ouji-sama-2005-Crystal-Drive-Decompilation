#include "types.h"

void func_ov004_020534cc(u8 *p, u16 a1, u16 a2)
{ u32 *r; u32 v; if (*(u32 *)p == 0) return; *(u16 *)(p + 0x30) = a1; *(u16 *)(p + 0x32) = a2;
  r = (u32 *)(p + 0x58); v = r[0]; r[0] = v | 8; }