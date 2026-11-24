#include "types.h"

#pragma thumb on
void IsDebugger(void)
{
    asm { swi 0x0f }
}
