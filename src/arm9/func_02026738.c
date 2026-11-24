#include "types.h"

typedef void (*Fn)(u32);

extern void func_02026710(void);
extern void func_0202497c(u32 a, Fn cb, u32 arg);
extern void func_01ff8504(u32 a, u32 b, u32 c, u32 flags);

void func_02026738(u32 a, u32 b, u32 c, u32 size, Fn cb, u32 arg)
{
    if (size == 0) {
        if (cb != 0)
            cb(arg);
        return;
    }
    func_02026710();
    if (cb != 0) {
        func_0202497c(a, cb, arg);
        func_01ff8504(a, b, c, (size >> 2) | 0xc4000000);
    } else {
        func_01ff8504(a, b, c, (size >> 2) | 0x84000000);
    }
}