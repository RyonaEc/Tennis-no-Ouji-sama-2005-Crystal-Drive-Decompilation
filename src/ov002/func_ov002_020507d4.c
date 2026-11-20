#include "types.h"

s32 func_ov002_020507d4(u8 *p)
{
    u32 v;

    v = *(u32 *)(p + 0xcc);
    switch (v) {
    case 0x68:
    case 0x9c:
    case 0xeb:
    case 0x14f:
    case 0x168:
        return 1;
    }
    return 0;
}