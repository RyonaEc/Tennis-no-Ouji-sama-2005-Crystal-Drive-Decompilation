#include "types.h"

#pragma thumb on
void Mod(void)
{
    asm { swi 0x09; mov r0, r1 }
}
