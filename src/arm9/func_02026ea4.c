#include "types.h"

extern u8 data_0204902c[];
extern void func_02027868(void);
extern void func_02027a34(void);

void func_02026ea4(void)
{
    if (*(u32 *)data_0204902c != 0) return;
    *(u32 *)data_0204902c = 1;
    func_02027868();
    func_02027a34();
}