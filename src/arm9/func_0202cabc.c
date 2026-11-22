#include "types.h"

typedef struct {
    u32 busy;
    u32 f4;
    u32 f8;
    u32 f0c;
    u32 f10;
    u32 f14;
    u32 f18;
} Ctx;

extern Ctx data_0204ad00;
extern u32 func_02026350(void);
extern void func_02026364(u32);
extern u32 func_0202cc04(void);

u32 func_0202cabc(u32 a0, u32 a1, u32 a2)
{
    u32 h;

    h = func_02026350();
    if (data_0204ad00.busy != 0) {
        func_02026364(h);
        return 1;
    }
    data_0204ad00.busy = 1;
    func_02026364(h);
    data_0204ad00.f14 = 1;
    data_0204ad00.f18 = 0;
    data_0204ad00.f8 = a0;
    data_0204ad00.f4 = a1;
    data_0204ad00.f10 = a2;
    if (func_0202cc04() != 0)
        return 0;
    return 3;
}