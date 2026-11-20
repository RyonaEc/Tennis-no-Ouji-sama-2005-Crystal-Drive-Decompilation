#include "types.h"

extern u32 data_02046630[];
extern u32 data_0204662c[];
extern u32 data_02042360[];

void func_02019e24(u32 a, u32 b)
{
    u32 obj = data_02046630[0];
    void (*fn)(u32, u32, u32, u32) = *(void (**)(u32, u32, u32, u32))(obj + 0x40);

    if (!fn)
        return;
    fn(data_0204662c[0], obj, b, data_02042360[0]);
}