#include "types.h"

typedef struct {
    u32 a;
    u32 b;
} P;

typedef struct {
    u32 p0[29];
    P p;
} S;

extern u8 *data_02047f00;

void func_0201d2dc(P *out) {
    *out = ((S *)data_02047f00)->p;
}