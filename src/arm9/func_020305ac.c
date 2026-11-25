#include "types.h"

typedef struct {
    u32 f0;
    u32 f4;
} S_020305ac;

extern u32 func_02026350(void);
extern u32 func_02024b9c(u32 a0);
extern u32 func_02024c58(u32 a0);
extern void func_02026364(u32 a0);
extern void WaitByLoop(u32 a0);

void func_020305ac(u32 a0, S_020305ac *s)
{
    for (;;) {
        s->f4 = func_02026350();
        s->f0 = func_02024b9c(0x27fffe8) & 0x40;
        if (s->f0 != 0)
            break;
        if (func_02024c58(a0) == 0)
            break;
        func_02026364(s->f4);
        WaitByLoop(1);
    }
}