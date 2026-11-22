#include "types.h"

typedef struct {
    u8 pad[0x14];
    u32 arr[16];
} S;

extern S data_02048d3c;

s32 func_020255b8(void)
{
    s32 i;

    for (i = 0; i < 0x10; i++) {
        if (data_02048d3c.arr[i] == 0)
            return i;
    }
    return -1;
}