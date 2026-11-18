#include "types.h"

#pragma thumb on
void RLUnCompReadByCallbackWrite16bit(void)
{
    asm { swi 0x15 }
}
