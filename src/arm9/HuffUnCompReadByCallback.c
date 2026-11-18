#include "types.h"

#pragma thumb on
void HuffUnCompReadByCallback(void)
{
    asm { swi 0x13 }
}
