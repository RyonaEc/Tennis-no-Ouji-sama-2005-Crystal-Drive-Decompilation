#include "types.h"

#pragma thumb on
void VBlankIntrWait(void)
{
    asm { mov r2, #0; swi 0x05 }
}
