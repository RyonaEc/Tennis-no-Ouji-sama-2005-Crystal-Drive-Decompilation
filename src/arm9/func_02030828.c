#include "types.h"

extern u8 data_0204c18c[];
extern void func_020263cc(u32 a0, u32 a1);
extern void func_02030808(void);

void func_02030828(u32 a0, u32 a1)
{
    if ((a1 & 0x3f) == 0x11) {
        u32 v = 0;

        if (*(void **)data_0204c18c != 0)
            v = ((u32 (*)(u32)) * (void **)data_0204c18c)(0);
        if (v == 0)
            return;
        func_02030808();
    } else {
        func_020263cc(a1 & 0x3f, a1);
    }
}