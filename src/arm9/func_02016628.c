#include "types.h"

typedef struct {
    u16 f0;
    u16 f2;
    u16 f4;
} T_02016628;

extern u32 data_02042afc;
extern u32 data_02042b04;
extern u8 data_02042300[];
extern u8 data_02043720[];
extern u32 func_020165a8(void);
extern void func_0201746c(void);
extern u32 func_02017544(void);
extern u32 func_020176f0(u32 a0);
extern u32 func_02017710(u32 a0);
extern u32 func_0202ffe4(void *a0, s32 a1, s32 a2, s32 a3, s32 a4);

void func_02016628(T_02016628 *p)
{
    u16 v;
    u32 r;
    u32 w;
    u32 arg;
    u32 res;

    v = p->f2;
    if (v != 0) {
        if (v == 0xf)
            return;
        if (v == 9)
            return;
        if (v == 0xd)
            return;
        func_020176f0(v);
        func_02017544();
        return;
    }
    switch (p->f4) {
    case 0xa:
        r = data_02042afc;
        if (r == 3) {
            w = data_02042b04;
            if (w == 6)
                return;
            if (w == 4) {
                if (func_020165a8() != 0)
                    return;
                func_0201746c();
                return;
            }
        } else if (r == 5) {
            arg = (u16)((1 << (*(u16 *)(data_02042300 + 0x10) + 1)) - 1);
            res = func_0202ffe4(data_02043720, 0xd, arg, 0x18, 1);
            if (res != 0) {
                func_020176f0(res);
                func_02017710(7);
            } else {
                func_02017710(5);
            }
            return;
        }
        func_02017710(4);
        return;
    case 0xb:
    case 0xc:
    case 0xd:
        return;
    }
}