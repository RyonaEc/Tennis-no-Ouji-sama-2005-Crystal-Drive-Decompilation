#include "types.h"

extern u32 func_0202585c(void);
extern u32 func_02027308(void);
extern s32 func_02027410(void);
extern s32 func_02027784(u32 a0);
extern u32 func_02027580(u32 a0);
extern void func_02027340(void);

u32 func_020276e8(u32 a0)
{
    u32 r;

    if (func_0202585c() != 0)
        return 0;
    r = func_02027308();
    if (r != 0)
        return r;
    if (a0 == 0)
        return 0;
    if (func_02027410() > 0) {
        while (func_02027784(0) != 0)
            ;
        r = func_02027308();
        if (r != 0)
            return r;
    } else {
        func_02027580(1);
    }
    func_02027340();
    for (;;) {
        func_02027784(1);
        r = func_02027308();
        if (r != 0)
            return r;
    }
}