#include "types.h"

typedef struct { u32 b:1; } BF;
extern u8 data_02042a68[];
u32 func_0201564c(void)
{
    return ((BF *)(*(u8 **)(data_02042a68 + 0x14)))->b;
}
