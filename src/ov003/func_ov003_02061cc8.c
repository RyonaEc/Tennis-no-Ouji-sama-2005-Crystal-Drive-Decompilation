#include "types.h"

typedef struct {
    u32 a;
    u32 b;
    u32 c;
    u32 d;
} Quad;

extern u8 data_ov003_020eb720[];
extern Quad data_ov003_020eb124;
extern u32 func_0202472c(u32 a0, u32 a1, u32 a2);
extern void func_02021bd8(u32 *w, u32 v, u32 t, s32 flag);
extern void func_02021b84(u32 *w, u32 v, u32 t, s32 flag);
extern s32 func_ov003_0205578c(u32 a0, u32 *a1);
extern u32 func_ov003_02055744(u32 a0, u32 *a1);
extern s32 func_ov003_020555fc(s32 a0);
extern void func_02021c70(u32 *s, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g, u32 h, u32 i);
extern void func_02021c2c(u32 *w, u32 v, s32 flags);
extern void func_02021ce4(u32 *h, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern u32 func_02021b54(u32 a0, u32 a1);
extern u32 func_02021d98(u32 a0, u32 a1, u32 a2);
extern void func_ov003_020641c8(u32 a0, u32 a1);
extern void func_02021d34(u32 *a0, u32 a1, u32 a2, u32 a3);
extern u32 func_02021b30(u32 a0);
extern u32 func_02024674(u8 *a0);
extern void func_02025c94(u32 a0, u32 a1);
extern void func_01ff854c(u32 a0, u32 a1, u32 a2);

u32 func_ov003_02061cc8(u32 a0, u32 a1, u8 *a2, u32 a3)
{
    u8 *p = *(u8 **)data_ov003_020eb720;
    Quad q;
    u32 r5, r4;
    s32 fp, x;
    u32 *w;
    u8 *b;
    u32 i;

    if (a2 == 0)
        return 0;

    i = a3 * 0x418;
    w = (u32 *)(p + 0x3c0);
    b = p + 0x3d4;
    w = (u32 *)((u8 *)w + i);
    b = b + i;

    func_0202472c((u32)w, (u32)b, 0x400);
    func_02021bd8(w, 0x7fff, 0x7fff, 1);
    func_02021b84(w, 0x7fff, 0x7fff, 0);

    r5 = func_ov003_0205578c(*(u16 *)(a2 + 4), (u32 *)(p + 0xb6dc));
    r4 = func_ov003_02055744(*(u16 *)(a2 + 4), (u32 *)(p + 0xb7dc));
    fp = func_ov003_020555fc(*(u16 *)(a2 + 0xc));
    x = func_ov003_020555fc(*(u16 *)(a2 + 0xe));

    func_02021c70(w, 3, 1, fp, x, 0, 0, 1, r5);
    func_02021c2c(w, r4, 3);
    func_02021ce4(w, 0, 0, 3, 0, 0x1f, 0);
    func_02021b54((u32)w, 1);

    q = data_ov003_020eb124;

    func_02021d98((u32)w, 0x22, q.b);
    func_ov003_020641c8((u32)w, 0);
    func_02021d34(w, 0x1000, -0x2a8, 0);

    func_02021d98((u32)w, 0x22, q.a);
    func_ov003_020641c8((u32)w, 0);
    func_02021d34(w, 0x1000, 0x2a8, 0);

    func_02021d98((u32)w, 0x22, q.c);
    func_ov003_020641c8((u32)w, 0);
    func_02021d34(w, -0x1000, 0x2a8, 0);

    func_02021d98((u32)w, 0x22, q.d);
    func_ov003_020641c8((u32)w, 0);
    func_02021d34(w, -0x1000, -0x2a8, 0);

    func_02021b30((u32)w);
    *(u32 *)(p + 0x7d4 + i) = func_02024674((u8 *)w);
    func_02025c94(*(u32 *)((u8 *)w + 8), *(u32 *)(p + 0x7d4 + i));
    func_01ff854c(3, (u32)b, *(u32 *)(p + 0x7d4 + i));
    return 1;
}