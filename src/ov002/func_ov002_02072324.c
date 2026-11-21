#include "types.h"

typedef unsigned char u8;
typedef unsigned short u16;

void func_ov002_02072324(u8 *p)
{
    *(u16 *)(p + 0xa) += 1;
}