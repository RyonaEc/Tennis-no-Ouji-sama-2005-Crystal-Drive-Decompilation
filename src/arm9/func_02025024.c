#include "types.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct OSThread OSThread;
typedef void (*OSSwitchThreadCallback)(OSThread *from, OSThread *to);

typedef struct {
    u16 isNeedRescheduling;
    u16 pad;
    u16 irqDepth;
    u16 pad2;
    OSThread *currentThread;
    u32 pad3;
    OSSwitchThreadCallback switchCallback;
} OSThreadInfo;

struct OSThread {
    char pad[0x68];
    OSThread *next;
    char pad2[4];
    u32 priority;
};

extern OSThreadInfo data_02048d3c;
extern OSThread **data_02048d34;
extern OSSwitchThreadCallback data_02048d30;

extern u32 func_020263a8(void);
extern OSThread *func_020250ec(void);
extern s32 func_0202564c(OSThread *t);
extern void func_02025698(OSThread *t);

void func_02025024(void)
{
    OSThreadInfo *info = &data_02048d3c;
    OSThread *currentThread;
    OSThread *nextThread;

    if (info->irqDepth > 0 || func_020263a8() == 0x12) {
        info->isNeedRescheduling = 1;
        return;
    }

    currentThread = *data_02048d34;
    nextThread = func_020250ec();

    if (currentThread == nextThread || !nextThread)
        return;

    if (func_0202564c(currentThread))
        return;

    if (data_02048d30)
        data_02048d30(currentThread, nextThread);

    if (info->switchCallback)
        info->switchCallback(currentThread, nextThread);

    data_02048d3c.currentThread = nextThread;

    func_02025698(nextThread);
}