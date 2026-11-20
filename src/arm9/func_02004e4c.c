#include "types.h"

typedef struct { u32 b0 : 1; u32 b1 : 1; u32 rest : 30; } F;
typedef struct { u16 a, b; } Q;
extern u8 data_02042a68[];
extern u8 data_02042aac[];
extern void func_0202b39c(void);
extern s32 func_0202b31c(u8 *);
extern void func_02004ed8(void);
void func_02004e4c(void)
{


    func_0202b39c();
    if (func_0202b31c(data_02042aac) != 0) {
        *(u16 *)(data_02042a68 + 0x48) = 0;
        *(u16 *)(data_02042a68 + 0x4a) = 3;
    }

    if ((((F *)*(u32 *)(data_02042a68 + 0x14))->b0) == 0 || (((F *)*(u32 *)(data_02042a68 + 0x14))->b1) == 0) {
        *(Q *)(data_02042a68 + 0x4c) = *(Q *)(data_02042a68 + 0x44);
        *(Q *)(data_02042a68 + 0x50) = *(Q *)(data_02042a68 + 0x48);
    }
    func_02004ed8();
}