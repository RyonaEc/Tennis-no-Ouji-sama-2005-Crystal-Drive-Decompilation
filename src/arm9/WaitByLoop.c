#include "types.h"

#pragma thumb on
void WaitByLoop(void)
{
    asm { swi 0x03 }
}
