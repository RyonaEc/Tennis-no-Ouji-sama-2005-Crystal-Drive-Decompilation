#include "types.h"

u32 func_02018808(u8 *p)
{
    return *(u32 *)p + (*(u16 *)(p + 8) << 4);
}
