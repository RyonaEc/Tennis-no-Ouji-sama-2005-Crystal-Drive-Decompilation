#include "types.h"

extern u32 func_02026710(u32, u32);
extern u32 func_02020c50(u32);
extern u8 data_02042370[];
extern u8 data_02048cb4[];
extern u8 data_02048cb8[];

u32 func_02024314(void)
{
    u32 t0, t1;
    if (*(u32 *)(data_02042370) != (-1)) {
        t0 = func_02026710(*(u32 *)(data_02042370), (-1));
    }
    t1 = func_02020c50(*(u32 *)(data_02048cb8));
    *(u32 *)(data_02048cb8) = 0x0;
    *(u32 *)(data_02048cb4) = 0x0;
    return (u32)data_02048cb4;
}
