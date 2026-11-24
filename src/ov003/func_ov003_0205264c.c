#include "types.h"

typedef unsigned short u16;
typedef unsigned char u8;

int func_ov003_0205264c(int a0, int a1, u16 *src, u8 *dst)
{
    u16 j, i;
    u16 *row;
    u16 c;

    for (i = 0; i < 0x11; i++) {
        row = (u16 *)(dst + i * 0x20);
        for (j = 0; j < 0x10; j++) {
            if (a0 == j) {
                c = (u16)(i + (src[j] & 0x1f));
                if (c > 0x1f) c = 0x1f;
                row[j] = c;
                c = (u16)(i + ((src[j] & 0x3e0) >> 5));
                if (c > 0x1f) c = 0x1f;
                row[j] += c << 5;
                c = (u16)(i + ((src[j] & 0x7c00) >> 10));
                if (c > 0x1f) c = 0x1f;
                row[j] += c << 10;
            } else {
                c = (u16)(src[j] & 0x1f);
                if (c > 0x1f) c = 0x1f;
                row[j] = c;
                c = (u16)((src[j] & 0x3e0) >> 5);
                if (c > 0x1f) c = 0x1f;
                row[j] += c << 5;
                c = (u16)((src[j] & 0x7c00) >> 10);
                if (c > 0x1f) c = 0x1f;
                row[j] += c << 10;
            }
        }
    }
    return 0;
}