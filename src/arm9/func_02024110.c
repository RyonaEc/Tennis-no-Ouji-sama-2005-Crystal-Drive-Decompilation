#include "types.h"

extern u32 func_02026710(u32, u32);
extern u32 func_02020b10(u32);
extern u8 data_02042370[];
extern u8 data_02048cac[];
extern u8 data_02048cbc[];

u32 func_02024110(void)
{
    u32 t0, t1;
    if (*(u32 *)(data_02042370) != (-1)) {
        t0 = func_02026710(*(u32 *)(data_02042370), (-1));
    }
    t1 = func_02020b10(*(u32 *)(data_02048cac));
    *(u32 *)(data_02048cac) = 0x0;
    *(u32 *)(data_02048cbc) = 0x0;
    return (u32)data_02048cbc;
}
