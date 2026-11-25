#include "types.h"

typedef unsigned int u32;
typedef signed int s32;

typedef struct OSThread OSThread;

struct OSThread {
    char pad[0x68];
    OSThread *next;
    char pad2[4];
    u32 priority;
};

typedef struct {
    char pad[0xc];
    OSThread *list;
} OSiThreadInfo;

extern OSiThreadInfo data_02048d3c;

extern u32 func_02026350(void);
extern void func_02026364(u32 a);
extern void func_02025550(OSThread *t);
extern void func_02025024(void);

s32 func_02024f70(OSThread *thread, u32 prio)
{
    OSThread *t = data_02048d3c.list;
    OSThread *pre = 0;
    u32 enable;

    enable = func_02026350();

    while (t && t != thread) {
        pre = t;
        t = t->next;
    }

    if (!t || t == (OSThread *)0x02048d90) {
        func_02026364(enable);
        return 0;
    }

    if (t->priority != prio) {
        if (!pre)
            data_02048d3c.list = thread->next;
        else
            pre->next = thread->next;

        thread->priority = prio;
        func_02025550(thread);
        func_02025024();
    }

    func_02026364(enable);

    return 1;
}