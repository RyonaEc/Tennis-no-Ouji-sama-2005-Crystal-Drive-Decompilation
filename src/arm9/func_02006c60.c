#include "types.h"

typedef struct { u32 b:1; } BF;
extern u8 data_02042a68[];
extern u8 data_0203d06c[];
extern void func_0200ffe0(u32);
void func_02006c60(void)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10);
    u32 n;
    if (!((BF *)p)->b) return;
    if (*(u8 *)(p + 0xb) != 0xb) return;
    n = *(u8 *)(p + 0xd0);
    if (*(u8 *)(data_0203d06c + n * 0x18) & 4) n--;
    func_0200ffe0(n);
}
