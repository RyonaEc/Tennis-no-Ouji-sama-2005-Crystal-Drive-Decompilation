#include "types.h"

typedef struct { u32 w[6]; } W24;

extern u8 data_02042358[];
extern u8 data_02042360[];
extern u8 data_02042364[];

void func_0201a07c(void)
{
    *(u32 *)(*(u32 *)data_02042360) = 0x1000;
    *(u32 *)(*(u32 *)data_02042360 + 4) = 0;
    *(u32 *)(*(u32 *)data_02042360 + 8) = 0;
    *(u32 *)(*(u32 *)data_02042360 + 0xc) = 0x1000;
    *(u32 *)(*(u32 *)data_02042360 + 0x10) = 0;
    *(u32 *)(*(u32 *)data_02042360 + 0x14) = 0;
    *(W24 *)*(u32 *)data_02042358 = *(W24 *)*(u32 *)data_02042360;
    *(u32 *)(*(u32 *)data_02042364) = 0;
}