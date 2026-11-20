#include "types.h"

extern u32 func_0201745c(void);
extern void func_02017544(void);

u32 func_0201508c(void)
{
    switch (func_0201745c())
    {
    case 1:
        return 1;
    case 3:
        break;
    case 7:
        func_02017544();
        break;
    }
    return 0;
}