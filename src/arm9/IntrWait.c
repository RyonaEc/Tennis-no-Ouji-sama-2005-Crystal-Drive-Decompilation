#include "types.h"

#pragma thumb on
void IntrWait(void)
{
    asm { swi 0x04 }
}
