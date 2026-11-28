#include "types.h"

extern u8 data_0203bbe8[];
extern u8 data_0203d06c[];
extern u8 data_02042a68[];
extern u32 func_0200ff30(u32 a0);
extern u32 func_02010150(u32 a0);

void func_0200880c(s32 a0, u16 *a1)
{
    u16 i;

    *a1 = 0;
    if (a0 >= 0xb)
        return;
    if (a0 >= 8) {
        for (i = 0; i < 0x17; i++) {
            if (a0 == *(u8 *)(data_0203bbe8 + i * 8 + 1)) {
                if (func_0200ff30(i)) {
                    u16 n = *a1;
                    *a1 = n + 1;
                    *(u16 *)((u8 *)a1 + n * 2 + 2) = i;
                }
            }
        }
    } else {
        for (i = 0; i < 0x3c; i++) {
            u8 *e = data_0203d06c + i * 0x18;

            if (a0 == *(u8 *)(e + 1)) {
                if (func_02010150(i)) {
                    if (*(u8 *)(*(u32 *)(data_02042a68 + 0x10) + 0xb) != 5
                        || (*(u8 *)e & 0x10) != 0) {
                        u16 n = *a1;
                        *a1 = n + 1;
                        *(u16 *)((u8 *)a1 + n * 2 + 2) = i;
                    }
                }
            }
        }
    }
}