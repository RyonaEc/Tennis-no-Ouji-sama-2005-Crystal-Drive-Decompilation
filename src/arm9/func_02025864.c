#include "types.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u16 queueSend;
    u16 queueReceive;
    void **msgArray;
    s32 msgCount;
    s32 firstIndex;
    s32 usedCount;
} OSMessageQueue;

extern u32 func_02026350(void);
extern void func_02026364(u32 a);
extern void func_020251c0(u16 *p);
extern void func_0202514c(u16 *p);

s32 func_02025864(OSMessageQueue *mq, void **msg, s32 flags)
{
    u32 enabled = func_02026350();

    while (mq->usedCount == 0) {
        if (!(flags & 1)) {
            func_02026364(enabled);
            return 0;
        }
        func_020251c0(&mq->queueReceive);
    }

    if (msg != 0)
        *msg = mq->msgArray[mq->firstIndex];

    mq->firstIndex = (mq->firstIndex + 1) % mq->msgCount;
    mq->usedCount--;

    func_0202514c(&mq->queueSend);

    func_02026364(enabled);
    return 1;
}