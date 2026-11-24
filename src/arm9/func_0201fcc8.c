#include "types.h"

#pragma thumb on

/* Nine fixed words of a display/DP block. The body is what retail has, byte for
   byte, except how 0x1000 reaches r3: retail forms it with `movs r3, #1` and
   `lsls r3, r3, #0xc`, and every 1.2 build (b56, base, sp2, sp2p3, sp3, sp4)
   instead loads it from a literal pool plus a trailing nop, which is 4 bytes
   longer. That is a 1.2-vs-2.0 materialisation policy, not a source shape: 20
   spellings of this statement were tried under b56 and all of them pooled. This
   unit therefore names 2.0/base in config/ATOJ/mwcc_versions.txt and the whole
   ROM digest still matches - see notes/mwccarm-codegen-log.md. */
void func_0201fcc8(u32 *p, u32 a, u32 b)
{
    p[0] = 0x1000;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = b;
    p[5] = a;
    p[6] = 0;
    p[7] = -a;
    p[8] = b;
}