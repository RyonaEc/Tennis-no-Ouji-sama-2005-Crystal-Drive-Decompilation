#include "types.h"

typedef void (*Fn)(void);
void func_ov009_0204dc10(void)
{
    Fn fn = *(Fn *)*(u32 *)0x02054f84;

    if (!fn)
        return;
    fn();
}
