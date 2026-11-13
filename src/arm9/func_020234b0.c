#include "types.h"

extern u8 data_02048c90[];
extern u32 func_020207cc(void);

void func_020234b0(void)
{
    *(u32 *)data_02048c90 = func_020207cc();
}
