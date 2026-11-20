#include "types.h"

typedef struct {
    u32 w[2];
    u8 n;
} E;

extern u8 data_0204ab00[];

void func_02027a34(void) {
    s32 i;
    E *e;

    for (i = 0; i < 8; i++) {
        e = (E *)data_0204ab00 + i;
        e->w[0] = 0;
        e->w[1] = 0;
        e->n = 0;
    }
}