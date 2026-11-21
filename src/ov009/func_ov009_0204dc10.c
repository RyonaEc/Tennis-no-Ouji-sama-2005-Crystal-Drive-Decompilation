#include "types.h"

extern u32 data_ov009_02054f84;

typedef void (*Fn)(void);
void func_ov009_0204dc10(void)
{
    Fn fn = *(Fn *)data_ov009_02054f84;

    if (!fn)
        return;
    fn();
}
