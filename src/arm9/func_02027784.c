#include "types.h"

extern u32 data_02049030;
extern u32 data_02049034;
extern u32 data_02049040;
extern u32 data_02049044;
extern u32 data_0204904c;
extern u32 data_02049054[];
extern u32 func_02027ae4(void);

u32 func_02027784(u32 a0)
{
    s32 idx;
    u8 *obj;
    u8 *tail;

    if (a0 == 0) {
        if (data_02049034 == func_02027ae4())
            return 0;
    } else {
        while (data_02049034 == func_02027ae4()) {
        }
    }
    idx = data_02049044;
    obj = (u8 *)data_02049054[idx];
    idx = idx + 1;
    data_02049044 = idx;
    if (idx > 8) {
        idx = 0;
        data_02049044 = idx;
    }
    tail = obj;
    while (*(u32 *)tail != 0)
        tail = *(u8 **)tail;
    if (data_02049040 != 0)
        *(u32 *)data_02049040 = (u32)obj;
    else
        data_02049030 = (u32)obj;
    data_02049040 = (u32)tail;
    data_0204904c = data_0204904c - 1;
    data_02049034 = data_02049034 + 1;
    return (u32)obj;
}