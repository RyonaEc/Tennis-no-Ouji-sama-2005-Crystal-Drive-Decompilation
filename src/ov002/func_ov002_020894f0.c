#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02088e28(void);
extern void func_ov002_02089484(s32 a0);
extern void func_ov002_02089210(s32 a0);
extern void func_ov002_02088f50(void);
extern void func_ov002_02088b0c(void);

void func_ov002_020894f0(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *w = p + 0x4bc4;
    s32 v;

    *(s32 *)(w + 0x354) = 0;
    func_ov002_02088e28();
    v = 0xf4 - *(s16 *)(w + 0x366) * 0xf4 / 100;
    if (v == 0xf4 && *(s32 *)(w + 0x364) > 0)
        v = 0xf3;
    func_ov002_02089484(v);
    v = (*(s16 *)(w + 0x366) + 0x13) / 20;
    if (v == 0 && *(s32 *)(w + 0x364) > 0)
        v = 1;
    func_ov002_02089210(v);
    func_ov002_02088f50();
    func_ov002_02088b0c();
    *(s32 *)(w + 0x364) -= *(s32 *)(w + 0x368);
    if (*(s32 *)(w + 0x364) < 0)
        *(s32 *)(w + 0x364) = 0;
}