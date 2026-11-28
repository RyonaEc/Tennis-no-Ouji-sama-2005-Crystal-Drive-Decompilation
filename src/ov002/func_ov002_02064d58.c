#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

extern u32 func_ov002_02058470(u8 *a0);
extern u32 func_ov002_02062dbc(u32 a0);

void func_ov002_02064d58(u32 a0)
{
    u8 *base = B;
    u8 *st = base + 0x1744 + a0 * 0x38;
    u8 *g = base + 0x1824;
    u32 k = *(u32 *)(g + 0xcc);
    u8 *e = base + 0x2e4 + a0 * 0x158;

    switch (k) {
    case 0xa4:
    case 0x151:
        st[0xb] = 4;
        return;
    }
    if (func_ov002_02058470(e) >= 6 || (*(u32 *)(g + 0xc8) & 0xc0) != 0) {
        switch (st[0]) {
        case 0:
            break;
        case 1:
            if (func_ov002_02062dbc(2) == 0) {
                st[0xb] = 7;
                return;
            }
            break;
        case 2:
            st[0xb] = 7;
            return;
        }
    }
    st[0xb] = 1;
}