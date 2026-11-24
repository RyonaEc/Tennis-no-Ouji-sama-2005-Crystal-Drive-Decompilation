#include "types.h"

#define OSi_ANYP_LOCK_ID_FLAG 0x027fffb0
#define OSi_ANYP_LOCK_ID_START 0x40

#pragma arm on

asm void func_02024b5c(register u16 lockID)
{
#pragma unused( lockID )

    ldr    r3, =OSi_ANYP_LOCK_ID_FLAG

    cmp    r0, #OSi_ANYP_LOCK_ID_START+32
    addpl  r3, r3, #4

    subpl  r0, r0, #OSi_ANYP_LOCK_ID_START+32
    submi  r0, r0, #OSi_ANYP_LOCK_ID_START

    mov r1, #0x80000000
    mov r1, r1, lsr r0

    ldr    r2, [r3, #0]
    orr    r2, r2, r1
    str    r2, [r3, #0]

    bx     lr
}