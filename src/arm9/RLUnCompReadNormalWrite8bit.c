#include "types.h"

#pragma thumb on
void RLUnCompReadNormalWrite8bit(void)
{
    asm { swi 0x14 }
}
