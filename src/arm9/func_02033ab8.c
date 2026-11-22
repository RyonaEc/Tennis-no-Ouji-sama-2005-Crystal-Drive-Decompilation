#include "types.h"

typedef struct {
    u32 w[4];
} Elem;

typedef struct {
    u32 count;
    u32 pad0[3];
    Elem elem[1];
} Info;

extern u8 data_02041fcc[];
extern u8 data_020420d8[];
extern void func_02032f38(u32, u32, u32, u32);

void func_02033ab8(Info *p, u32 i)
{
    u32 n;

    if (i >= p->count)
        func_02032f38((u32)data_02041fcc, 0x55, (u32)data_020420d8, 1);
    p->count = p->count - 1;
    n = p->count;
    goto test;
body:
    p->elem[i] = p->elem[i + 1];
    i++;
test:
    if (i < n)
        goto body;
}