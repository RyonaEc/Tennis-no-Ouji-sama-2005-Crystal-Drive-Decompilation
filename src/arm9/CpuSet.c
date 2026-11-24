#include "types.h"

#pragma thumb on
void CpuSet(void)
{
    asm { swi 0x0b }
}
