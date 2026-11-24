#include "types.h"

extern u32 func_0202637c(void);
extern u32 func_02026350(void);
extern void func_02026390(u32 last);
extern void func_02026364(u32 last);

s32 func_02024d38(u32 lockID, u8 *lockp, void (*ctrlFuncp)(void), u32 disableFIQ)
{
    u32 lastInterrupts;

    if (lockID != *(u16 *)(lockp + 4))
        return -2;

    if (disableFIQ)
        lastInterrupts = func_0202637c();
    else
        lastInterrupts = func_02026350();

    *(u16 *)(lockp + 4) = 0;

    if (ctrlFuncp)
        ctrlFuncp();

    *(u32 *)lockp = 0;

    if (disableFIQ)
        func_02026390(lastInterrupts);
    else
        func_02026364(lastInterrupts);

    return 0;
}