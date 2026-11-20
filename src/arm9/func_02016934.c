#include "types.h"

extern u8 data_02042b04[];
extern u8 data_02042ce0[];
extern u8 data_02042b40[];
extern void func_0201610c(void);
extern s32 func_02016818(void);
extern s32 func_0202eb28(s32 (*)(void), u8 *, u8 *);
extern void func_020176f0(void);

s32 func_02016934(void)
{
    if (*(u32 *)data_02042b04 - 4 <= 2) return 1;
    func_0201610c();
    if (func_0202eb28(func_02016818, data_02042ce0, data_02042b40) == 2) return 1;
    func_020176f0();
    return 0;
}