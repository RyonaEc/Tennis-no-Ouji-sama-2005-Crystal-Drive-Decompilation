#include "types.h"

void func_02030888(s32 a, s32 b)
{
    s32 t = b & 0x3f;

    if (t == 1) {
        *(u16 *)0x0204c180 = 1;
        return;
    }
    func_020263cc(t);
}
