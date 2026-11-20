#include "types.h"

typedef struct {
    u32 f : 1;
    u32 r : 31;
} B;

extern u8 data_02042a68[];
extern s32 func_0202e718(void);
extern s32 func_0201745c(void);
extern void func_0201484c(void);

s32 func_02014ca0(void)
{
    if (((B *)*(u32 *)(data_02042a68 + 0x14))->f == 0) return 0;
    *(u32 *)(*(u32 *)(data_02042a68 + 0x14) + 4) = func_0202e718();
    switch (func_0201745c()) {
    case 5:
        func_0201484c();
        return *(u32 *)(*(u32 *)(data_02042a68 + 0x14) + 8);
    case 8:
        return 1;
    }
    return 0;
}