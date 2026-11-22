#include "types.h"

typedef struct {
    u32 f0;
    u32 f4;
    u32 f8;
    u32 f0c;
    u32 f10;
    u32 f14;
    u32 f18;
    u32 f1c;
} Ctx;

extern u16 data_0204acfc;
extern Ctx data_0204ad00;
extern void func_02027d68(void);
extern u32 func_02027f4c(u32, u32);
extern void func_02027f74(u32, u32);

void func_0202cb38(void)
{
    if (data_0204acfc != 0)
        return;
    data_0204acfc = 1;
    data_0204ad00.f0 = 0;
    data_0204ad00.f4 = 0;
    data_0204ad00.f1c = 0;
    data_0204ad00.f8 = 0;
    data_0204ad00.f0c = 0;
    func_02027d68();
    while (func_02027f4c(5, 1) == 0)
        ;
    func_02027f74(5, 0x0202c4dc);
}