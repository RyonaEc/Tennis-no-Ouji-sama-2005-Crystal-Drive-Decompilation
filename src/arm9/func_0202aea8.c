#include "types.h"

typedef struct {
    u16 x;
    u16 y;
    u16 touch;
    u16 validity;
} TPData;

typedef struct {
    s32 x0;
    s32 xDotSize;
    s32 xDotSizeInv;
    s32 y0;
    s32 yDotSize;
    s32 yDotSizeInv;
} TPiCalibrateParam;

extern u8 data_0204ac68[];
extern TPiCalibrateParam data_0204ac80;

void func_0202aea8(TPData *disp, const TPData *raw)
{
    TPiCalibrateParam *cal;

    if (*(u16 *)(data_0204ac68 + 0x30) == 0) {
        *disp = *raw;
        return;
    }

    cal = &data_0204ac80;

    disp->touch = raw->touch;
    disp->validity = raw->validity;

    if (raw->touch == 0) {
        disp->x = 0;
        disp->y = 0;
        return;
    }

    disp->x = (u16)(((u64)(raw->x << 2) - cal->x0) * cal->xDotSizeInv >> 22);

    if ((s16)disp->x < 0) {
        disp->x = 0;
    } else if ((s16)disp->x > 0xff) {
        disp->x = 0xff;
    }

    disp->y = (u16)(((u64)(raw->y << 2) - cal->y0) * cal->yDotSizeInv >> 22);

    if ((s16)disp->y < 0) {
        disp->y = 0;
    } else if ((s16)disp->y > 0xbf) {
        disp->y = 0xbf;
    }
}