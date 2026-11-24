#include "types.h"

extern u32 func_02027488(u32 a);
extern u32 func_02027784(u32 v);
extern void func_02027340(void);

void func_020274f0(u32 a)
{
    if (func_02027488(a) != 0)
        return;
    while (func_02027784(0) != 0)
        ;
    if (func_02027488(a) != 0)
        return;
    func_02027340();
    if (func_02027488(a) != 0)
        return;
    do {
        func_02027784(1);
    } while (func_02027488(a) == 0);
}