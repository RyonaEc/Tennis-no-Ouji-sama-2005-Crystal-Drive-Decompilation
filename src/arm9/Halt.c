#include "types.h"

#pragma thumb on
void Halt(void)
{
    asm { swi 0x06 }
}
