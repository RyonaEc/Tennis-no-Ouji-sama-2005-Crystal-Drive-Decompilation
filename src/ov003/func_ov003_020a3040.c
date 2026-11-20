#include "types.h"

typedef struct {
    u8 head[0x54];
    u8 tail[0xc];
} Blk;

extern void func_02006d24(u8 *buf);

s32 func_ov003_020a3040(s32 a0, u8 *a1, s32 a2) {
    Blk b;

    func_02006d24(b.head);
    if (a2 == 0x31 || a2 == 0x2f) {
        if (a1[(s32)b.tail + a0 * 3] != 0) {
            if (a2 == 0x2f)
                a2 = 0x2e;
            else if (a2 == 0x31)
                a2 = 0x30;
        }
        return a2;
    }
    return a2;
}