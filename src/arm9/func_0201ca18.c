#include "types.h"

typedef union {
    u32 w;
    struct {
        u32 b0 : 1;
        s32 f : 1;
        u32 rest : 30;
    } b;
} Flags;

typedef struct {
    u8 pad0[0x24];
    Flags flags;
    u8 pad28[0x40 - 0x28];
    u32 f40;
    u32 f44;
} Obj;

extern u32 func_020271d4(u32, u32, u32, u32);
extern void func_0202b934(u8 *);
extern void func_0202b91c(u8 *);
extern u32 func_020274c4(void);
extern void func_02027580(u32);
extern void func_020274f0(u32);
extern void func_0201c9e0(Obj *);

void func_0201ca18(Obj *p)
{
    u32 h;

    if (p->flags.b.f) {
        func_020271d4(p->f44, 0, 1 << p->f40, 0);
        func_0202b934((u8 *)p + 8);
        func_0202b91c((u8 *)p + 0x14);
        p->flags.w &= ~2;
        h = func_020274c4();
        func_02027580(1);
        func_020274f0(h);
    }
    func_0201c9e0(p);
}