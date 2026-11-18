#include "types.h"

typedef void (*Fn)(void);
void func_ov005_0204f42c(void)
{
    Fn fn = *(Fn *)*(u32 *)0x020c9740;

    if (!fn)
        return;
    fn();
}
