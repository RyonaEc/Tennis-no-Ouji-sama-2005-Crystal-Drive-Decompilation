#include "types.h"

extern u8 data_02042afc[];
extern u8 data_02042aec[];
extern u8 data_02042300[];
extern void func_02017710(u32);
extern u32 func_02015574(void);
extern void func_020171dc(void);

u32 func_02017550(u32 a0)
{
    u16 v;

    *(u32 *)data_02042afc = a0;
    func_02017710(3);
    if (a0 == 0 || a0 == 2 || a0 == 4)
    {
        v = *(u16 *)data_02042aec;
        *(u16 *)(data_02042300 + 0xc) = v;
        *(u16 *)data_02042aec = v + 1;
        if (func_02015574() != 0)
            *(u16 *)(data_02042300 + 0x10) = 1;
        else
            *(u16 *)(data_02042300 + 0x10) = 7;
        func_020171dc();
    }
    else
    {
        return 0;
    }
}