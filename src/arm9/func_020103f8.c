#include "types.h"

typedef struct {
    u32 w0;
    u32 w1;
} P2;

extern u8 data_02042a68[];

void func_020103f8(P2 *a0) {
    u8 *p;

    p = *(u8 **)(data_02042a68 + 0x10);
    p = *(u8 **)(p + 0x154);
    *(P2 *)(p + 0x4a4) = *a0;
}