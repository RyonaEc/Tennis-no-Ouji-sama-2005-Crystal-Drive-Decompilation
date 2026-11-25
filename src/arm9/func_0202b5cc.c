#include "types.h"

typedef struct {
    u32 f0;
    u8 pad4[8];
    u16 fc;
    u8 pade[2];
    u32 f10;
    u8 pad14[0x1c];
    u16 f30;
    u16 f32;
    u16 f34;
    u16 f36;
} S_0204ac68;

extern u16 data_0204ac64;
extern S_0204ac68 data_0204ac68;

extern u32 func_02027d68(void);
extern s32 func_02027f4c(s32 bit, s32 i);
extern void func_02027f74(u32 tag, void *callback);
extern void func_0202b65c(void);

void func_0202b5cc(void)
{
    if (data_0204ac64 != 0)
        return;
    data_0204ac64 = 1;
    func_02027d68();
    data_0204ac68.f32 = 0;
    data_0204ac68.f36 = 0;
    data_0204ac68.fc = 0;
    data_0204ac68.f0 = 0;
    data_0204ac68.f10 = 0;
    data_0204ac68.f30 = 0;
    data_0204ac68.f34 = 0;
    while (func_02027f4c(6, 1) == 0)
        ;
    func_02027f74(6, (void *)func_0202b65c);
}
