#include "types.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u16 state;
} WMStatus;

typedef struct {
    u8 pad[4];
    WMStatus *status;
} WMArm9Buf;

extern u32 func_0202e2d0(void);
extern void func_02025c5c(u16 *p, u32 n);
extern WMArm9Buf *data_0204b6ec;

u32 func_0202e1cc(s32 paramNum, ...)
{
    u32 result;
    u16 now;
    u32 temp;
    u32 *ap;

    result = func_0202e2d0();
    if (result != 0)
        return result;

    func_02025c5c(&data_0204b6ec->status->state, 2);
    now = data_0204b6ec->status->state;

    result = 3;
    ap = (u32 *)(((u32)&paramNum & ~3u) + 4);
    for (; paramNum; paramNum--) {
        temp = *(u32 *)((ap += 1) - 1);
        if (temp == now)
            result = 0;
    }
    return result;
}