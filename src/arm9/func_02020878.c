#include "types.h"

extern u8 data_02048c74[];
extern void func_02021538(u32);
u32 func_02020878(u16 *p)
{
    u16 v = *p;
    *p = 0;
    *(u16 *)data_02048c74 = *(u16 *)data_02048c74 | v;
    func_02021538(v);
    return v;
}
