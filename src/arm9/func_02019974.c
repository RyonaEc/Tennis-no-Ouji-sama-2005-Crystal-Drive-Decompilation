#include "types.h"

extern u8 data_0204662c[];
extern u32 func_0201a538(void);
extern void func_0201a56c(u32);
extern void func_0201a548(u32);
extern void func_02019b68(u32);
extern void func_0201a558(void);

void func_02019974(u32 a0)
{
    if (*(u32 *)(*(u32 *)data_0204662c + 0x40) != 0) {
        u32 s = func_0201a538();

        func_0201a56c(1);
        func_0201a548(*(u32 *)(*(u32 *)data_0204662c + 0x40));
        func_02019b68(a0);
        func_0201a56c(0);
        func_0201a548(s);
        func_0201a558();
    } else {
        func_02019b68(a0);
    }
}