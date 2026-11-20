#include "types.h"

extern u32 func_0201745c(void);
extern void func_0201746c(void);
extern void func_02017544(void);

u32 func_020135fc(void)
{
    switch (func_0201745c())
    {
    case 1:
        return 1;
    case 5:
        func_0201746c();
        break;
    case 7:
        func_02017544();
        break;
    }
    return 0;
}