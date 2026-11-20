#include "types.h"

typedef struct {
    u32 pad : 3;
    u32 f : 1;
    u32 rest : 28;
} B;

extern u8 data_02042a68[];

void func_02014c5c(void)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x14);

    if (((B *)p)->f == 0) return;
    switch (*(u16 *)(p + 0x10)) {
    case 1:
        *(u16 *)(p + 0x12) = 2;
        return;
    case 2:
        *(u16 *)(p + 0x12) = 1;
        break;
    }
}