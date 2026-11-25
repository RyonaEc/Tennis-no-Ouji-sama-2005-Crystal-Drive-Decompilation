#include "types.h"

typedef struct {
    u32 *f0;
    u32 f4;
    u32 f8;
    u32 fc;
    u16 f10[2];
    u32 f14;
} S_0204aec0;

extern S_0204aec0 data_0204aec0;
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_020263cc(void);
extern void func_0202514c(u8 *a0);

void func_0202cdc8(u32 a0, u32 a1)
{
    S_0204aec0 *s;
    u32 lock;

    s = &data_0204aec0;
    lock = func_02026350();
    if (s->f8 != a0 || s->fc == 0) {
        func_020263cc();
    } else {
        if (s->f14 != a1)
            func_020263cc();
        s->fc = s->fc - 1;
        if (s->fc == 0) {
            s->f8 = 0xfffffffd;
            s->f14 = 0;
            func_0202514c((u8 *)s->f10);
        }
    }
    *(u32 *)s->f0 = 0;
    func_02026364(lock);
}