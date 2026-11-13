#include "types.h"

extern u8 data_02048ca8[];
extern u32 func_020207a4(void);

void func_020233cc(void)
{
    *(u32 *)data_02048ca8 = func_020207a4();
}
