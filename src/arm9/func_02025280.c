#include "types.h"

extern u32 data_02048d50[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern s32 func_020255b8(void);
extern void func_02025550(u32 a0);
extern void func_020255e8(u32 a0, u32 a1, u32 a2);
extern void func_02025228(void);
extern void func_020048e8(u32 data, u32 destp, u32 size);

typedef struct OSThread {
    u8 context[0x64];
    u32 state;
    u32 unk68;
    u32 id;
    u32 priority;
    u32 profiler;
    u32 mutex;
    u32 mutexQueueHead;
    u32 mutexQueueTail;
    u32 stackTop;
    u32 stackBottom;
    u32 stackWarningOffset;
    u16 joinQueue;
} OSThread;

void func_02025280(OSThread *thread, u32 func, u32 arg, u32 stack, u32 stackSize, u32 prio)
{
    u32 enable;
    s32 index;
    volatile u32 zero;

    enable = func_02026350();
    index = func_020255b8();
    if (index < 0) {
        func_02026364(enable);
        return;
    }
    thread->priority = prio;
    thread->id = (u32)index;
    thread->state = 0;
    thread->profiler = 0;
    data_02048d50[index] = (u32)thread;
    func_02025550((u32)thread);
    thread->stackBottom = stack;
    thread->stackTop = stack - stackSize;
    thread->stackWarningOffset = 0;
    *(u32 *)(thread->stackBottom - 4) = 0xfddb597d;
    *(u32 *)(thread->stackTop) = 0x7bf9dd5b;
    *(u16 *)((u8 *)thread + 0x90) = 0;
    func_020255e8((u32)thread, func, stack - 4);
    *(u32 *)((u8 *)thread + 4) = arg;
    *(u32 *)((u8 *)thread + 0x3c) = (u32)func_02025228;
    zero = 0;
    func_020048e8(zero, stack - stackSize + 4, stackSize - 8);
    thread->mutex = 0;
    thread->mutexQueueHead = 0;
    thread->mutexQueueTail = 0;
    func_02026364(enable);
}