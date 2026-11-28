#include "types.h"

extern u32 data_02048f80;
extern u32 func_02025e48(u32);
extern u32 func_02025dc0(u32, u32);
extern u32 func_02025dd4(u32);
extern u32 func_02025dac(u32, u32);

void func_02025f64(void)
{
    if (data_02048f80)
        return;

    data_02048f80 = 1;

    func_02025dc0(0, func_02025e48(0));
    func_02025dac(0, func_02025dd4(0));

    func_02025dac(2, 0);
    func_02025dc0(2, 0);

    func_02025dc0(3, func_02025e48(3));
    func_02025dac(3, func_02025dd4(3));

    func_02025dc0(4, func_02025e48(4));
    func_02025dac(4, func_02025dd4(4));

    func_02025dc0(5, func_02025e48(5));
    func_02025dac(5, func_02025dd4(5));

    func_02025dc0(6, func_02025e48(6));
    func_02025dac(6, func_02025dd4(6));
}