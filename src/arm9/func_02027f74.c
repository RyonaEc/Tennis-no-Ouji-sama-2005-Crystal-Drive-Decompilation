#include "types.h"

extern void *data_0204ab68[];
extern u32 func_02026350(void);
extern void func_02026364(u32 enabled);

void func_02027f74(u32 tag, void *callback)
{
    u32 enabled = func_02026350();
    volatile u32 *p = (volatile u32 *)0x027ffc00;

    data_0204ab68[tag] = callback;

    if (callback)
        p[0xe2] |= 1u << tag;
    else
        p[0xe2] &= ~(1u << tag);

    func_02026364(enabled);
}