#include "types.h"

extern u32 func_02021538(u32);
extern u8 data_02048c74[];

u32 func_02020af0(u32 a0)
{
    *(u16 *)((u8 *)data_02048c74) = (*(u16 *)((u8 *)data_02048c74) | a0);
    return func_02021538(a0);
}
