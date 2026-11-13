#include "types.h"

extern u8 data_02042a68[];

void func_02015558(void)
{
    *(u32 *)(*(u32 *)((u8 *)data_02042a68 + 20)) = (*(u32 *)(*(u32 *)((u8 *)data_02042a68 + 20)) | 0x10);
}
