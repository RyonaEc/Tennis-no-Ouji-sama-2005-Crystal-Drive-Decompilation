#include "types.h"

typedef struct {
    u32 b0 : 1;
    u32 rest : 31;
} F;

extern u8 data_02042a68[];
extern s32 func_0201745c(void);
extern void func_0202e95c(u8 *);
extern void func_0202ef50(void (*)(void));
extern void func_020154d4(void);
extern void func_020160f0(void);
extern void func_0201746c(void);
extern void func_02017544(void);

s32 func_020153ec(void)
{
    u8 buf[0x934];

    if (((F *)*(u32 *)(data_02042a68 + 0x14))->b0 != 0) {
        *(u32 *)(*(u32 *)(data_02042a68 + 0x14)) &= ~8;
        *(u32 *)(*(u32 *)(data_02042a68 + 0x14)) &= ~0x10;
        switch (func_0201745c()) {
        case 1:
            func_0202e95c(buf);
            switch (*(u16 *)buf) {
            case 2:
                func_0202ef50(func_020154d4);
                break;
            case 5:
                func_020160f0();
                break;
            }
            break;
        case 2:
            func_020160f0();
            break;
        case 5:
            func_0201746c();
            break;
        case 3:
            break;
        case 0:
        case 4:
        case 6:
        case 7:
        default:
            func_02017544();
            break;
        }
        return 0;
    }
    return 1;
}