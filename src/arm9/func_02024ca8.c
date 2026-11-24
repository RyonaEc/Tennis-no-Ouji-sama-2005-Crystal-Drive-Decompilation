#include "types.h"

extern u32 func_0202637c(void);
extern u32 func_02026350(void);
extern s32 func_02026cb8(u32 lockID, void *lockp);
extern void func_02026390(u32 last);
extern void func_02026364(u32 last);

s32 func_02024ca8(u32 lockID, u8 *lockp, void (*ctrlFuncp)(void), u32 disableFIQ)
{
    s32 lastLockFlag;
    u32 lastInterrupts;

    if (disableFIQ)
        lastInterrupts = func_0202637c();
    else
        lastInterrupts = func_02026350();

    lastLockFlag = func_02026cb8(lockID, lockp);

    if (lastLockFlag == 0) {
        if (ctrlFuncp)
            ctrlFuncp();
        *(u16 *)(lockp + 4) = (u16)lockID;
    }

    if (disableFIQ)
        func_02026390(lastInterrupts);
    else
        func_02026364(lastInterrupts);

    return lastLockFlag;
}