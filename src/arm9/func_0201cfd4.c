#include "types.h"

extern u8 data_02047eb0[];
extern u8 data_02047e9c[];

extern void func_020271d4(u32 a0, u32 a1, u32 a2, u32 a3);
extern u32 func_020274c4(void);
extern u32 func_02027580(u32 a0);
extern void func_020274f0(u32 a0);
extern s32 func_02025864(void *mq, void **msg, s32 flags);
extern void func_0201bea0(u32 a0);
extern void func_0201bebc(u32 mask);
extern void func_0201be3c(s32 a0);
extern void func_02026f9c(u32 a, s32 b, s32 c, s32 d);

void func_0201cfd4(void)
{
    s32 *s = (s32 *)data_02047eb0;
    s32 nl;
    u32 mask;
    u32 v;

    if (s[0] == 0)
        return;
    nl = (s[10] >= 0);
    mask = nl ? (1u << s[10]) : 0;
    func_020271d4(s[8], s[9], mask, 0);
    if (nl) {
        v = func_020274c4();
        func_02027580(1);
        func_020274f0(v);
        while (func_02025864(data_02047e9c, 0, 0) != 0)
            ;
    }
    if (s[9] != 0)
        func_0201bea0(s[9]);
    if (s[7] != 0)
        func_0201bebc(s[7]);
    if (nl)
        func_0201be3c(s[10]);
    if (s[1] == 1)
        func_02026f9c(0, 0, 0, 0);
    s[0] = 0;
}