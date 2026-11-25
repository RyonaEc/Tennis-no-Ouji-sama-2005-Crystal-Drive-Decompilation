#include "types.h"

typedef struct {
    u8 pad0[0x58];
    u16 raw_x1;
    u16 raw_y1;
    u8 dx1;
    u8 dy1;
    u16 raw_x2;
    u16 raw_y2;
    u8 dx2;
    u8 dy2;
} S_0202b540;

extern u32 func_0202afd8(void *cal, u16 raw_x1, u16 raw_y1, u16 dx1, u16 dy1,
                         u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2);

u32 func_0202b540(void *cal)
{
    S_0202b540 *s = (S_0202b540 *)0x027ffc80;
    u16 x1;
    u16 y1;
    u8 dx1;
    u16 x2;
    u16 y2;
    u8 dy1;
    u8 dx2;
    u8 dy2;

    x1 = s->raw_x1;
    y1 = s->raw_y1;
    dx1 = s->dx1;
    dy1 = s->dy1;
    x2 = s->raw_x2;
    y2 = s->raw_y2;
    dx2 = s->dx2;
    dy2 = s->dy2;

    if (x1 == 0 && x2 == 0 && y1 == 0 && y2 == 0)
        return 0;
    return func_0202afd8(cal, x1, y1, dx1, dy1, x2, y2, dx2, dy2) == 0;
}