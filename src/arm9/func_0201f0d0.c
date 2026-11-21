#include "types.h"

typedef struct T T;
struct T { u8 pad[0x118]; s32 f118; u8 f11c; u8 pad2[7]; u32 f124; u32 f128; };

extern u8 data_02047f34[];
extern void func_02025b88(u8 *);
extern void func_0201f38c(T *);
extern void func_0201cdb8(T *);
extern void func_02025b14(u8 *);

void func_0201f0d0(u32 id, u32 unused, T *t)
{
    if (id != t->f124)
        return;
    func_02025b88(data_02047f34);
    func_0201f38c(t);
    t->f124 = 0;
    t->f128 = 0;
    t->f11c = 0;
    if (t->f118 > 0) {
        func_0201cdb8(t);
        t->f118 = 0;
    }
    func_02025b14(data_02047f34);
}