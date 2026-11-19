#include "types.h"

extern void func_02029c8c(void *a0);
extern u32 func_02029930(void *a0, void *a1);
extern void func_02029828(void *a0, u32 a1, u32 a2);
extern u32 func_020297bc(void *a0, u32 a1, u32 a2);
extern void func_020298e4(void *a0);

extern u8 data_ov003_020eb07c[];
extern u32 data_ov003_020eb724;

void func_ov003_0205e804(void)
{
    u8 buf[0x44];

    func_02029c8c(buf);
    if (func_02029930(buf, data_ov003_020eb07c) == 0)
        return;
    func_02029828(buf, data_ov003_020eb724 + 0x140000,
                  *(u32 *)(buf + 0x24) - *(u32 *)(buf + 0x20));
    func_020297bc(buf, 0, 1);
    func_020298e4(buf);
}