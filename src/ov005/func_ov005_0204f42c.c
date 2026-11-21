#include "types.h"

extern u32 data_ov005_020c9740;

typedef void (*Fn)(void);
void func_ov005_0204f42c(void)
{
    Fn fn = *(Fn *)data_ov005_020c9740;

    if (!fn)
        return;
    fn();
}
