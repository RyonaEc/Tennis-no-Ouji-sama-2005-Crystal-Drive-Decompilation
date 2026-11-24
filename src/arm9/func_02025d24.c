#include "types.h"

extern u8 *func_02025ef4(u32 id);
extern u32 func_02025f08(u32 id);
extern void func_02025dac(u32 id, u32 lo);

#define ROUND(x, align) (((u32)(x) + (align) - 1) & ~((align) - 1))

void *func_02025d24(u32 id, u32 size, u32 align)
{
    u8 *ptr;
    u8 *arenaLo;

    ptr = func_02025ef4(id);
    if (!ptr)
        return 0;

    ptr = (u8 *)ROUND(ptr, align);
    arenaLo = ptr + size;
    arenaLo = (u8 *)ROUND(arenaLo, align);

    if ((u32)arenaLo > func_02025f08(id))
        return 0;

    func_02025dac(id, (u32)arenaLo);

    return ptr;
}