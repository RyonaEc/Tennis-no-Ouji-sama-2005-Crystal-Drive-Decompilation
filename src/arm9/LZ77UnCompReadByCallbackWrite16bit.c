#include "types.h"

#pragma thumb on
void LZ77UnCompReadByCallbackWrite16bit(void)
{
    asm { swi 0x12 }
}
