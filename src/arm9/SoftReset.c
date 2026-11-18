#include "types.h"

#pragma thumb on
void SoftReset(void)
{
    asm { swi 0x00 }
}
