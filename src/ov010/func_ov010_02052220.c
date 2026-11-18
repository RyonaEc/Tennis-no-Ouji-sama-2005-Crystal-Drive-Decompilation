#include "types.h"

extern u8 data_ov010_0205e500[];

typedef struct {
    u16 v[28];
} Blk;

void func_ov010_02052220(s32 idx, Blk *src)
{
    Blk *d = (Blk *)(*(u8 **)data_ov010_0205e500 + idx * 0x74 + 0x21c);

    *d = *src;
}