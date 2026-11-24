#include "types.h"

void func_020354f0(u8 *p, s32 n)
{
    u8 *q;
    u8 *d;
    u8 *z;
    u8 c;
    u32 carry;
    s32 len;

    if (n < 0) {
zero:
        *(u16 *)(p + 2) = 0;
        p[4] = 1;
        p[5] = 0x30;
        return;
    }
    len = p[4];
    if (n >= len)
        return;

    q = p + 5;
    d = q + n + 1;
    c = (u8)(*--d - 0x30);
    if (c == 5) {
        z = q + len;
        while (--z > d && *z == 0x30)
            ;
        if (z == d)
            carry = d[-1] & 1;
        else
            carry = 1;
    } else {
        carry = c > 5;
    }
    while (n != 0) {
        c = (u8)(*--d - 0x30 + carry);
        carry = c > 9;
        if (carry || c == 0)
            n--;
        else {
            *d = (u8)(c + 0x30);
            break;
        }
    }

    if (carry) {
        *(s16 *)(p + 2) += 1;
        p[4] = 1;
        p[5] = 0x31;
        return;
    }
    if (n == 0)
        goto zero;
    p[4] = n;
}