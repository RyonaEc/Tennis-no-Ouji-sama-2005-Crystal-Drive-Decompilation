#include "types.h"

#pragma thumb on
void LZ77UnCompReadNormalWrite8bit(void)
{
    asm { swi 0x11 }
}
