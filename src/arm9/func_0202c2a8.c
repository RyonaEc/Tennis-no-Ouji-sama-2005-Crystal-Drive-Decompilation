#include "types.h"

typedef struct {
    u32 a;
    u32 b;
} B_0204acb8;

typedef struct {
    u16 f0;
    u8 pad[6];
} E_0204acdc;

extern u16 data_0204aca0;
extern B_0204acb8 data_0204acb8;
extern E_0204acdc data_0204acdc[];
extern u32 data_0204acb0;
extern u32 data_0204acc8;

extern u32 func_02027d68(void);
extern s32 func_02027f4c(s32 bit, s32 i);
extern void func_02027f74(u32 tag, void *callback);
extern void func_0202c1bc(void);
extern u32 func_02025c1c(u32 a0);

void func_0202c2a8(void)
{
    s32 i;

    if (data_0204aca0 != 0)
        return;
    data_0204aca0 = 1;
    data_0204acb8.a = 0;
    data_0204acb8.b = 0;
    func_02027d68();
    while (func_02027f4c(8, 1) == 0) {
    }
    func_02027f74(8, func_0202c1bc);
    for (i = 0; i < 4; i++)
        data_0204acdc[i].f0 = 0;
    func_02025c1c((u32)&data_0204acc8);
    data_0204acb0 = *(u32 *)0x027ffc3c;
}