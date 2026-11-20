#include "types.h"

extern u8 data_02047620[];

void func_0201c63c(s32 a0, s32 a1) {
    *(u32 *)(data_02047620 + a0 * 0x1c) = (u16)a1;
}