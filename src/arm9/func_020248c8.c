#include "types.h"

#define reg_OS_IME (*(volatile u16 *)0x04000208)
#define reg_OS_IE  (*(volatile u32 *)0x04000210)

/* NitroSDK OS_DisableIrqMask (os_interrupt.c). OS_DisableIrq and OS_RestoreIrq
   are the SDK's inline IME save/restore, expanded here the way they inline. */
u32 func_020248c8(u32 intr)
{
    u32 ime = reg_OS_IME;
    u32 prep;
    u32 bak;

    reg_OS_IME = 0;
    prep = reg_OS_IE;
    reg_OS_IE = prep & ~intr;
    bak = reg_OS_IME;
    reg_OS_IME = ime;
    return prep;
}