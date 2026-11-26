#include "types.h"

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
} Tri;

extern Tri data_ov009_02054f44;
extern Tri data_ov009_02054f50;
extern void func_020269b8(u16, u16 *, s32);
extern void func_02022994(Tri *, Tri *, Tri *, s32, s32);

void func_ov009_0204f93c(void)
{
    u16 hdr[3];
    u16 *q = &hdr[1];

    u32 buf[0xc];
    Tri s1;
    Tri s2;
    Tri s3;
    u32 ip;

    q[0] = 0;
    q[1] = 0;
    hdr[0] = 0;
    func_020269b8(hdr[0], (u16 *)buf, 0x30);
    *(u16 *)0x04000060 = (*(u16 *)0x04000060 & ~0x3000) | 8;
    *(u32 *)0x04000448 = 1;
    s1 = data_ov009_02054f50;
    ip = (u32)&s2;
    *(u32 *)ip = 0;
    *(u32 *)(ip + 4) = 0;
    *(u32 *)(ip + 8) = 0;
    s3 = data_ov009_02054f44;
    func_02022994(&s1, &s3, (Tri *)ip, 1, 0);
    *(u32 *)0x040004c8 = 0x80000;
    *(u32 *)0x040004cc = 0x7fff;
}