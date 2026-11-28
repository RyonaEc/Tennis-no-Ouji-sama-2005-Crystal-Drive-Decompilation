#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203c0f0[];
extern u32 func_0200fe54(u32 a0);

typedef struct {
    u32 b0 : 1;
    u32 rest : 31;
} Bits;

void func_02008a90(u16 *out)
{
    u16 cnt[6] = { 0 };
    u16 i;
    u8 *p;
    u16 n;

    *out = 0;
    if (((Bits *)(*(u8 **)(data_02042a68 + 0x10)))->b0) {
        for (i = 0; i < 0x24; i++) {
            if (func_0200fe54(i) != 0) {
                p = data_0203c0f0 + i * 0xc;
                cnt[p[1]]++;
            }
        }
        for (i = 0; i < 5; i++) {
            if (cnt[i] != 0) {
                n = *out;
                *out = n + 1;
                *(u16 *)((u8 *)out + n * 2 + 2) = i;
            }
        }
    } else {
        for (i = 0; i < 5; i++) {
            n = *out;
            *out = n + 1;
            *(u16 *)((u8 *)out + n * 2 + 2) = i;
        }
    }
}