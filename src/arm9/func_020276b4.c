#include "types.h"

extern u32 data_02049038;
extern u32 data_0204903c;

void func_020276b4(u32 *p)
{
    if (data_0204903c == 0) {
        data_0204903c = (u32)p;
        data_02049038 = (u32)p;
    } else {
        *(u32 *)data_0204903c = (u32)p;
        data_0204903c = (u32)p;
    }
    *p = 0;
}