#include "types.h"

extern u8 *data_ov005_020c9740;
extern void func_020269b8(u32 a0, u32 a1, u32 a2);
extern void func_ov005_0204c914(void);

u32 func_ov005_0204dc7c(u32 x)
{
    u8 *p = data_ov005_020c9740;
    u32 k = 0;
    volatile u16 z;

    if (*(u16 *)(p + 0x11e) != 0)
        goto end;
    switch (x) {
    case 0x59:
    case 0x5a:
        k = 1;
        break;
    }
    if (k != 0) {
        z = 0;
        func_020269b8(z, 0x0660c800, 0x1200);
        func_ov005_0204c914();
    }
end:
    return 0;
}