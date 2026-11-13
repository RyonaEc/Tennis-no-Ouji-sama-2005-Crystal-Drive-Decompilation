#include "types.h"

typedef struct {
    u32 f0x0;
    u32 f0x4;
    u32 f0x8;
    u32 f0xc;
    u32 f0x10;
    u32 f0x14;
    u32 f0x18;
    u32 f0x1c;
    u8 pad_32[64];
    u32 f0x60;
    u32 f0x64;
} S_0202ad34;

u32 func_0202ad34(u32 a0)
{
    if (a0 == 0x0) return 0x1;
    ((S_0202ad34 *)a0)->f0x14 = 0x0;
    ((S_0202ad34 *)a0)->f0x18 = 0x0;
    ((S_0202ad34 *)a0)->f0x1c = 0x0;
    ((S_0202ad34 *)a0)->f0x0 = 0x67452301;
    ((S_0202ad34 *)a0)->f0x4 = 0xefcdab89;
    ((S_0202ad34 *)a0)->f0x8 = 0x98badcfe;
    ((S_0202ad34 *)a0)->f0xc = 0x10325476;
    ((S_0202ad34 *)a0)->f0x10 = 0xc3d2e1f0;
    ((S_0202ad34 *)a0)->f0x60 = 0x0;
    ((S_0202ad34 *)a0)->f0x64 = 0x0;
    return 0x0;
}
