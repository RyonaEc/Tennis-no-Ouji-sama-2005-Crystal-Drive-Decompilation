#include "types.h"

extern u8 data_02042a68[];
extern u32 func_0201745c(void);

typedef struct
{
    u32 on : 1;
} F;

u32 func_02015668(void)
{
    if (((F *)*(u32 *)(data_02042a68 + 0x14))->on)
    {
        if (func_0201745c() != 0)
            return *(u32 *)(*(u32 *)(data_02042a68 + 0x14) + 4) + 1;
    }
    return 0;
}