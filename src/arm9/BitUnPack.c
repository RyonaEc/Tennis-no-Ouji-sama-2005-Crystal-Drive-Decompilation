#include "types.h"

#pragma thumb on
void BitUnPack(void)
{
    asm { swi 0x10 }
}
