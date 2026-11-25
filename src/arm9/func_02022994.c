#include "types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} VecFx32;

extern void func_0201fe0c(VecFx32 *v, VecFx32 *p);
extern void func_0201ff64(const VecFx32 *a, const VecFx32 *b, VecFx32 *p);
extern s32 func_02020000(const VecFx32 *a, const VecFx32 *b);

void func_02022994(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, u32 draw, s32 *mtx)
{
    VecFx32 vLook, vRight, vUp;
    s32 tmp1, tmp2, tmp3;
    s32 *p;

    vLook.x = camPos->x - target->x;
    vLook.y = camPos->y - target->y;
    vLook.z = camPos->z - target->z;
    func_0201fe0c(&vLook, &vLook);

    func_0201ff64(camUp, &vLook, &vRight);
    func_0201fe0c(&vRight, &vRight);

    func_0201ff64(&vLook, &vRight, &vUp);

    if (draw) {
        *(volatile u32 *)0x04000440 = 2;
        p = (s32 *)0x0400045c;
        *p = vRight.x;
        *p = vUp.x;
        *p = vLook.x;

        *p = vRight.y;
        *p = vUp.y;
        *p = vLook.y;

        *p = vRight.z;
        *p = vUp.z;
        *p = vLook.z;
    }

    tmp1 = -func_02020000(camPos, &vRight);
    tmp2 = -func_02020000(camPos, &vUp);
    tmp3 = -func_02020000(camPos, &vLook);

    if (draw) {
        *p = tmp1;
        *p = tmp2;
        *p = tmp3;
    }

    if (mtx != 0) {
        mtx[0] = vRight.x;
        mtx[1] = vUp.x;
        mtx[2] = vLook.x;
        mtx[3] = vRight.y;
        mtx[4] = vUp.y;
        mtx[5] = vLook.y;
        mtx[6] = vRight.z;
        mtx[7] = vUp.z;
        mtx[8] = vLook.z;
        mtx[9] = tmp1;
        mtx[0xa] = tmp2;
        mtx[0xb] = tmp3;
    }
}