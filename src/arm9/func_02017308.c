#include "types.h"

extern u8 data_02043f40[];
extern u32 func_0202e4a4(u8 *, u32);
extern void func_020176f0(void);
extern void func_02017710(u32);
extern u32 func_020172cc(void);

u32 func_02017308(void)
{
    if (func_0202e4a4(data_02043f40, 3) != 0)
    {
        func_020176f0();
        return 0;
    }
    func_02017710(3);
    return func_020172cc() != 0;
}