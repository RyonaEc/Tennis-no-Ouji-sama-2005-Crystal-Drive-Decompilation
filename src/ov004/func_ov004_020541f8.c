#include "types.h"

extern u8 data_02042a68[];
extern u8 data_ov004_02063758[];

extern u32 func_02029cb4(void);
extern void func_02029cc4(s32 a);
extern u32 func_02029768(u32 a0);
extern u32 func_02029c8c(void *a0);
extern s32 func_02029930(void *a0, u32 a1);
extern u8 *func_02017b60(u8 *a0, s32 a1, s32 a2);
extern u32 func_02029828(void *a0, void *a1, s32 a2);
extern void func_02017b20(u8 *a0, s32 a1);
extern s32 func_020298e4(void *a0);

typedef struct {
    u8 pad0[0x20];
    s32 start;
    s32 end;
    u8 pad1[0x20];
} Ctx;

s32 func_ov004_020541f8(u32 a0)
{
    Ctx c;
    s32 n;
    u8 *p;

    while (func_02029cb4() == 0)
        func_02029cc4(3);
    func_02029768((u32)data_ov004_02063758);
    func_02029c8c(&c);
    if (func_02029930(&c, a0) != 0) {
        n = c.end - c.start;
        p = func_02017b60(*(u8 **)data_02042a68, n, 0x20);
        if (p != 0 && func_02029828(&c, p, n) != n) {
            func_02017b20(*(u8 **)data_02042a68, (s32)p);
            p = 0;
        }
        func_020298e4(&c);
    }
    return (s32)p;
}