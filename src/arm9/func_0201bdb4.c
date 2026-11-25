#include "types.h"

typedef struct {
    u32 fn;
    u32 arg;
} S_cb;

extern s32 data_020475c8;
extern S_cb data_020475d8;
extern S_cb data_020475cc;
extern void func_02026ea4(void);
extern void func_0201bce0(void);
extern void func_0201bcd4(void);
extern u32 func_0202b964(u32 a0);
extern u32 func_0202b94c(u32 a0);
extern u32 func_0201bf48(void);
extern u32 func_0201d180(void);
extern void func_0201c7a0(void);

void func_0201bdb4(void)
{
    if (data_020475c8 != 0)
        return;
    data_020475c8 = 1;
    func_02026ea4();
    data_020475d8.fn = (u32)func_0201bce0;
    data_020475d8.arg = 0;
    data_020475cc.fn = (u32)func_0201bcd4;
    data_020475cc.arg = 0;
    func_0202b964((u32)&data_020475d8);
    func_0202b94c((u32)&data_020475cc);
    func_0201bf48();
    func_0201d180();
    func_0201c7a0();
}