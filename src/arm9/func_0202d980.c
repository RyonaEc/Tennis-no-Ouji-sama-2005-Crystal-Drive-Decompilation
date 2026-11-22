#include "types.h"

typedef struct {
    u32 f00;
    u32 f04;
    u32 f08;
    u32 f0c;
    u32 f10;
    u32 f14;
    u32 f18;
    u32 f1c;
    s32 f20;
    u32 f24;
} Base;

extern Base data_0204aec0;
extern void func_020266c0(u32);
extern void func_0202d9f4(void);
extern void func_020248c8(u32);
extern void func_02024894(u32);
extern void func_0202daec(void);

void func_0202d980(void)
{
    func_020266c0(data_0204aec0.f24);
    data_0204aec0.f18 += 0x200;
    data_0204aec0.f1c += 0x200;
    data_0204aec0.f20 -= 0x200;
    if (data_0204aec0.f20 == 0) {
        func_020248c8(0x80000);
        func_02024894(0x80000);
        func_0202daec();
        return;
    }
    func_0202d9f4();
}