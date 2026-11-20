#include "types.h"

extern u8 data_02049010[];
extern u8 data_02049014[];
extern u8 data_02049018[];
extern u8 data_0204901c[];
extern void func_020248c8(u32);

void func_020262cc(void)
{
    if (*(u16 *)data_02049010 != 0) return;
    *(u16 *)data_02049010 = 1;
    *(u32 *)data_0204901c = 0;
    *(u32 *)(data_0204901c + 4) = 0;
    func_020248c8(4);
    *(u32 *)data_02049018 = 0;
    *(u32 *)data_02049014 = 0;
}