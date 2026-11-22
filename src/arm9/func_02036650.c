#include "types.h"

u8 *func_02036650(u8 *s, u32 c)
{
    u8 ch = *s;
    u8 cc = (u8)c;
    u8 *p = s + 1;

    if (ch != 0) {
        do {
            if (ch == cc)
                return p - 1;
            ch = *p++;
        } while (ch != 0);
    }
    if (cc != 0)
        return 0;
    return p - 1;
}