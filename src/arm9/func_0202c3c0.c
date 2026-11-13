#include "types.h"

typedef struct {
    u32 f0x0;
} S_0202c3c0;

u32 func_0202c3c0(u32 a0, u32 a1)
{
    ((S_0202c3c0 *)a1)->f0x0 = a0;
    return a0;
}
