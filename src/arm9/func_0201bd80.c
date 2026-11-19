#include "types.h"

extern s32 func_02027784(u32);
extern void func_0201c65c(void);
extern void func_0201d0d0(void);
extern void func_0201f7ac(void);
extern void func_02027580(u32);

void func_0201bd80(void)
{
    while (func_02027784(0) != 0)
        ;
    func_0201c65c();
    func_0201d0d0();
    func_0201f7ac();
    func_02027580(0);
}