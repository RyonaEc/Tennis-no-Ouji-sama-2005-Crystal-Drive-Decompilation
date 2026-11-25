#include "types.h"

extern u32 func_02026350(void);
extern void func_020251c0(u32 *p);
extern void func_02026364(u32 bak);

u32 func_02028d00(u8 *p_arc)
{
    u32 bak = func_02026350();
    u32 bak_stat;
    u32 suspended;

    suspended = (*(volatile u32 *)(p_arc + 0x10) & 8) ? 1 : 0;
    bak_stat = suspended ? 0 : 1;

    if (bak_stat) {
        u32 running = (*(volatile u32 *)(p_arc + 0x10) & 0x10) ? 1 : 0;

        if (running) {
            *(volatile u32 *)(p_arc + 0x10) |= 0x40;
            do {
                func_020251c0((u32 *)(p_arc + 0xe));
            } while ((((*(volatile u32 *)(p_arc + 0x10) & 0x40) ? 1 : 0) != 0));
        } else {
            *(volatile u32 *)(p_arc + 0x10) |= 8;
        }
    }

    func_02026364(bak);

    return bak_stat;
}