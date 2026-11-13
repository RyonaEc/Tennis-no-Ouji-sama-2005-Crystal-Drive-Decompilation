#include "types.h"

extern u8 data_02042a68[];

typedef struct {
    u32 f0x0;
} S_02006d00;

u32 func_02006d00(u32 a0, u32 a1)
{
    u16 v0;
    ((S_02006d00 *)a0)->f0x0 = *(u16 *)((u8 *)(*(u32 *)(data_02042a68 + 16)) + 12);
    v0 = *(u16 *)((u8 *)(*(u32 *)(data_02042a68 + 16)) + 14);
    ((S_02006d00 *)a1)->f0x0 = v0;
    return v0;
}
