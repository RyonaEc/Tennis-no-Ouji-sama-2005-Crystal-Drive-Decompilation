#include "types.h"

#pragma thumb on
void Div(void)
{
    asm { swi 0x09 }
}
