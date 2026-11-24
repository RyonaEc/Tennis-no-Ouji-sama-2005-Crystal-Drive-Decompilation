#include "types.h"

typedef s32 fx32;
typedef s64 fx64;

typedef struct {
    fx32 x;
    fx32 y;
    fx32 z;
} VecFx32;

#define FX64_SHIFT 12
#define FX32_SHIFT 12

void func_0201ff64(const VecFx32 * a, const VecFx32 * b, VecFx32 * axb)
{
    fx32 x, y, z;

    x = (fx32) (((fx64) a->y * b->z -
                 (fx64) a->z * b->y +
                 (1 << (FX64_SHIFT - 1))) >> FX64_SHIFT);

    y = (fx32) (((fx64) a->z * b->x -
                 (fx64) a->x * b->z +
                 (1 << (FX64_SHIFT - 1))) >> FX64_SHIFT);

    z = (fx32) (((fx64) a->x * b->y -
                 (fx64) a->y * b->x +
                 (1 << (FX64_SHIFT - 1))) >> FX32_SHIFT);

    axb->x = x;
    axb->y = y;
    axb->z = z;
}