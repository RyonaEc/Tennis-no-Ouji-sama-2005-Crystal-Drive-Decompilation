#include "types.h"

typedef struct {
    u16 f00;
    u8 f02 : 4;
    u8 f03;
    u8 f04;
    u8 f05;
    u8 data06[0x14];
    u8 f1a;
    u8 data1b[0x50 - 0x1b];
    u8 f50;
    u8 pad51[0x64 - 0x51];
    u16 f64 : 3;
} S;

extern void func_020269d0(void *, void *, u32);

void func_02026564(u8 *out)
{
    S *p = (S *)0x027ffc80;

    out[0] = p->f64;
    out[1] = p->f02;
    out[2] = p->f03;
    out[3] = p->f04;
    *(u16 *)(out + 0x18) = p->f1a;
    *(u16 *)(out + 0x4e) = p->f50;
    func_020269d0((u8 *)p + 6, out + 4, 0x14);
    func_020269d0((u8 *)p + 0x1c, out + 0x1a, 0x34);
}