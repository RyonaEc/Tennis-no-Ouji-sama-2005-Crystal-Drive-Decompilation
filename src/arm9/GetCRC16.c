#include "types.h"

#pragma thumb on
void GetCRC16(void)
{
    asm { swi 0x0e }
}
