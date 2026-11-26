#include "types.h"

/* PXI bus bring-up on the ARM9: the SDK's PXI_InitFifo with this build's
   constants (FIFO_CNT = 0xc408, 32 tags, timeout 1000, 5 sync rounds). */

extern u16 data_0204ab64;
extern void *data_0204ab68[];

extern s32 func_02026350(void);
extern void func_02026364(s32 mode);
extern void func_02024a54(u32 intrBit, void *function);
extern u32 func_02024910(u32 a0);
extern void func_02027d74(void);

void func_02027fe8(void)
{
    s32 enabled;
    s32 i;
    s32 timeout;
    u32 c;
    u8 *p = (u8 *)0x027ffc00;

    enabled = func_02026350();

    if (data_0204ab64 == 0) {
        data_0204ab64 = 1;

        *(s32 *)(p + 0x388) = 0;

        for (i = 0; i < 32; i++)
            data_0204ab68[i] = 0;

        func_02024a54(0x40000, (void *)func_02027d74);
        func_02024910(0x40000);

        *(volatile u16 *)0x04000184 = 0xc408;

        for (i = 0;; i++) {
            c = *(volatile u16 *)0x04000180 & 0xf;
            *(volatile u16 *)0x04000180 = (u16)(c << 8);

            if (c == 0 && i > 4)
                break;

            for (timeout = 1000; (*(volatile u16 *)0x04000180 & 0xf) == c;
                 timeout--) {
                if (timeout == 0) {
                    i = 0;
                    break;
                }
            }
        }
    }

    func_02026364(enabled);
}