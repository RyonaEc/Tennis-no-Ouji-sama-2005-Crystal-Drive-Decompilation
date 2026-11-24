#include "types.h"

#pragma thumb on
void Sqrt(void)
{
    asm { swi 0x0d }
}
