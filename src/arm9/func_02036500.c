#include "types.h"

extern u8 data_0204c44c[];
extern u8 data_02048d3c[];
extern u32 data_0204c378[];
extern u32 data_0204c39c[];
extern u32 data_0204c774[];
extern s32 func_02025a54(u8 *a0);
extern void func_02025b88(u8 *a0);
extern void func_02025b14(u8 *a0);
extern void func_02033dec(u32 x);

s32 func_02036500(s32 a0)
{
    u32 i;
    u32 fn;

    if (a0 < 1 || a0 > 7)
        return -1;
    if (func_02025a54(data_0204c44c) == 0) {
        data_0204c378[7] = *(u32 *)(*(u32 *)(data_02048d3c + 8) + 0x6c);
        data_0204c39c[7] = 1;
    } else if (data_0204c378[7] == *(u32 *)(*(u32 *)(data_02048d3c + 8) + 0x6c)) {
        data_0204c39c[7] += 1;
    } else {
        func_02025b88(data_0204c44c);
        data_0204c378[7] = *(u32 *)(*(u32 *)(data_02048d3c + 8) + 0x6c);
        data_0204c39c[7] = 1;
    }
    i = a0 - 1;
    fn = data_0204c774[i];
    if (fn != 1)
        data_0204c774[i] = 0;
    if (--data_0204c39c[7] == 0)
        func_02025b14(data_0204c44c);
    if (fn == 1 || (fn == 0 && a0 == 1))
        return 0;
    if (fn == 0)
        func_02033dec(0);
    ((void (*)(u32))fn)(a0);
    return 0;
}