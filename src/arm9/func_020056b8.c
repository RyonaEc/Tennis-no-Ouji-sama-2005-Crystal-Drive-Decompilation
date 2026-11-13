#include "types.h"

extern u8 data_02042a68[];
typedef struct { u32 v[155]; } B;
extern u32 func_02005734(u32);
u32 func_020056b8(B *dst)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x15c);
    if (func_02005734(2) == 0) return 0;
    if (*(u8 *)p == 0) return 0;
    *dst = *(B *)p;
    return 1;
}
