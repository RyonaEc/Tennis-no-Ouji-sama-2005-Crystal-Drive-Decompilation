#include "types.h"

#pragma thumb on
void CpuFastSet(void)
{
    asm { swi 0x0c }
}
