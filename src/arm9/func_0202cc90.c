#include "types.h"

typedef struct {
    u32 *cmd;
    u8 pad4[0x30];
    u32 flag;
    u8 pad38[0x9c];
    u16 busyQ[4];
} S_0204aec0;

extern S_0204aec0 data_0204aec0;
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_020251c0(u16 *p);

BOOL func_0202cc90(void)
{
    S_0204aec0 *const p = &data_0204aec0;
    u32 bak;

    bak = func_02026350();

    while ((p->flag & 4) != 0)
        func_020251c0(p->busyQ);

    func_02026364(bak);

    return *(u32 *)p->cmd == 0;
}