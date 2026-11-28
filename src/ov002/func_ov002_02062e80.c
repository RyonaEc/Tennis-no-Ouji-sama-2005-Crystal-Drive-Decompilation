#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 data_ov002_020fc694[];
extern u8 *func_ov002_020848d0(void);
extern void func_ov002_02082034(void);
extern void func_ov002_0204e878(void);
extern void func_ov002_0206e734(void);
extern void func_ov002_0206f374(void);
extern void func_ov002_0206e514(void);
extern void func_ov002_0206de70(void);

void func_ov002_02062e80(void)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 *s = base + 0x36ac;
    u8 *g;
    u32 f;

    g = func_ov002_020848d0();
    f = *(u32 *)(base + 0x28);
    if (f == 0 || ((f & 8) && !(f & 1))) {
        if (s[0x24] != 0)
            s[0x24]--;
    }
    *(u16 *)(g + 4) = (data_ov002_020fc694[s[0x24]] << 12) + (data_ov002_020fc694[s[0x24]] << 8) + (data_ov002_020fc694[s[0x24]] << 4) + data_ov002_020fc694[s[0x24]];
    func_ov002_02082034();
    *(u32 *)g &= ~0xee00;
    func_ov002_0204e878();
    *(u16 *)(g + 6) = 0x28bf;
    *(u16 *)(g + 8) = 0x0a08;
    *(u16 *)(g + 0xa) = 0;
    g[0x24] = 0x3f;
    g[0x25] = 0x3f;
    g[0x26] = 0x3f;
    g[0x27] = 0x1f;
    func_ov002_0206e734();
    func_ov002_0206f374();
    func_ov002_0206e514();
    func_ov002_0206de70();
}