#include "types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} VecFx32;

typedef struct {
    u16 z;
    u16 pad;
    u8 buf[0x30];
} Buf;

extern u8 data_ov003_020eb004[];
extern u8 data_ov003_020eaff8[];
extern void func_020269b8(u16 v, u8 *dst, s32 count);
extern void func_02022994(const VecFx32 *camPos, const VecFx32 *camUp,
                          const VecFx32 *target, u32 draw, s32 *mtx);
extern void func_ov003_0205e0d4(u32 a0);

void func_ov003_0205e228(void)
{
    Buf b;
    VecFx32 camPos;
    VecFx32 target;
    VecFx32 camUp;

    b.z = 0;
    func_020269b8(b.z, b.buf, 0x30);
    *(u16 *)0x04000060 = (*(u16 *)0x04000060 & ~0x3000) | 8;
    *(u32 *)0x04000448 = 1;
    camPos = *(VecFx32 *)data_ov003_020eb004;
    {
        s32 *tp = (s32 *)&target;

        tp[0] = 0;
        tp[1] = 0;
        tp[2] = 0;
    }
    camUp = *(VecFx32 *)data_ov003_020eaff8;
    func_02022994(&camPos, &camUp, &target, 1, 0);
    *(u32 *)0x040004c8 = 0x80000;
    *(u32 *)0x040004cc = 0x7fff;
    func_ov003_0205e0d4(0);
}