#include "types.h"

extern u8 data_02042afc[];
extern u8 data_02042af8[];
extern u8 data_02042fa0[];
extern u8 data_02042ce0[];
extern void func_02017710(u32);
extern void func_02026a50(void *, void *, u32);
extern void func_02025c94(void *, u32);
extern void func_02016934(void);

void func_02016030(u32 a0)
{
    *(u32 *)data_02042afc = 5;
    func_02017710(3);
    func_02026a50(data_02042fa0 + a0 * 0xc0, data_02042ce0, 0xc0);
    func_02025c94(data_02042ce0, 0xc0);
    *(u16 *)data_02042af8 = 0;
    func_02016934();
}