#include "types.h"

typedef unsigned int u32;

extern u32 data_0204c274;
extern void (*data_0204c26c)(void);
extern void func_0203b640(void);
extern void func_02033cb8(u32 x);

void func_02033dec(u32 x)
{
    if (data_0204c274 == 0) {
        func_0203b640();
        if (data_0204c26c != 0) {
            data_0204c26c();
            data_0204c26c = 0;
        }
    }
    func_02033cb8(x);
}