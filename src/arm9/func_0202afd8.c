#include "types.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed int s32;
typedef signed short s16;

typedef struct {
    s16 x0;
    s16 y0;
    s16 xDotSize;
    s16 yDotSize;
} TPCal;

extern u32 func_02026350(void);
extern void func_02026364(u32 a);

#define IN_S16_RANGE(x) ((x) < 0x8000 && (x) >= -0x8000)

u32 func_0202afd8(TPCal *calibrate, u16 raw_x1, u16 raw_y1, u16 dx1, u16 dy1,
                  u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2)
{
    s32 rx_width, dx_width, ry_width, dy_width;
    s32 tmp32;
    u32 enabled;

    if (raw_x1 >= 0x1000 || raw_y1 >= 0x1000 || raw_x2 >= 0x1000 || raw_y2 >= 0x1000)
        return 1;

    if (dx1 >= 0x100 || dx2 >= 0x100 || dy1 >= 0xc0 || dy2 >= 0xc0)
        return 1;

    if (dx1 == dx2 || dy1 == dy2 || raw_x1 == raw_x2 || raw_y1 == raw_y2)
        return 1;

    rx_width = raw_x1 - raw_x2;
    dx_width = dx1 - dx2;

    enabled = func_02026350();

    *(volatile u16 *)0x04000280 = 0;
    *(volatile u32 *)0x04000290 = rx_width << 8;
    *(volatile u64 *)0x04000298 = (u32)dx_width;

    ry_width = raw_y1 - raw_y2;
    dy_width = dy1 - dy2;

    while (*(volatile u16 *)0x04000280 & 0x8000) {
    }
    tmp32 = *(volatile s32 *)0x040002a0;

    *(volatile u16 *)0x04000280 = 0;
    *(volatile u32 *)0x04000290 = ry_width << 8;
    *(volatile u64 *)0x04000298 = (u32)dy_width;

    if (!IN_S16_RANGE(tmp32)) {
        func_02026364(enabled);
        return 1;
    }
    calibrate->xDotSize = (s16)tmp32;

    tmp32 = (s16)((((s32)(raw_x1 + raw_x2) << 8)
                   - ((s32)(dx1 + dx2) * calibrate->xDotSize))
                  >> 7);

    if (!IN_S16_RANGE(tmp32)) {
        func_02026364(enabled);
        return 1;
    }
    calibrate->x0 = (s16)tmp32;

    while (*(volatile u16 *)0x04000280 & 0x8000) {
    }
    tmp32 = *(volatile s32 *)0x040002a0;
    func_02026364(enabled);

    if (!IN_S16_RANGE(tmp32))
        return 1;
    calibrate->yDotSize = (s16)tmp32;

    tmp32 = (s16)((((s32)(raw_y1 + raw_y2) << 8)
                   - ((s32)(dy1 + dy2) * calibrate->yDotSize))
                  >> 7);

    if (!IN_S16_RANGE(tmp32))
        return 1;
    calibrate->y0 = (s16)tmp32;

    return 0;
}