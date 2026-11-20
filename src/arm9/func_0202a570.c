#include "types.h"

typedef struct {
    u32 a;
    u32 b;
} P;

extern u8 data_0204ac14[];

void func_0202a570(P *out, u8 *src) {
    P tmp;

    tmp.a = (u32)data_0204ac14;
    tmp.b = *(u32 *)(src + 0x18);
    *out = tmp;
}