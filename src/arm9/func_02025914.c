#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_020251c0(u16 *p);
extern void func_0202514c(u8 *a0);

typedef struct {
    u8 pad0[4];
    u32 *msgArray;
    s32 msgCount;
    s32 firstIndex;
    s32 usedCount;
} Q;

u32 func_02025914(Q *q, u32 a1, u32 a2)
{
    u32 enabled;
    s32 last;

    enabled = func_02026350();
    while (q->msgCount <= q->usedCount) {
        if ((a2 & 1) == 0) {
            func_02026364(enabled);
            return 0;
        }
        func_020251c0((u16 *)q);
    }
    last = (q->firstIndex + q->usedCount) % q->msgCount;
    q->msgArray[last] = a1;
    q->usedCount++;
    func_0202514c((u8 *)q + 2);
    func_02026364(enabled);
    return 1;
}