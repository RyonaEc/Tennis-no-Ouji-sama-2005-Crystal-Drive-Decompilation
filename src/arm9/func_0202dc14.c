#include "types.h"

typedef struct {
    s32 result;             /* 0x00 */
} CARDiCommandArg;

typedef struct {
    CARDiCommandArg *cmd;   /* 0x00 */
    s32 command;            /* 0x04 */
    u8 pad[0x34 - 0x08];
    u32 flag;               /* 0x34 */
} CARDiCommon;

extern s32 func_02027f4c(s32 bit, s32 i);
extern void func_020263c0(u32 cycle);
extern void func_02025c94(u32 a0, u32 a1);
extern void func_02025cb0(void);
extern s32 func_02027e98(s32 fifotag, u32 data, s32 err);
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_020251c0(u16 *p);

static inline void cardi_send_pxi(u32 data)
{
    while (func_02027e98(0xb, data, 1) < 0)
        ;
}

BOOL func_0202dc14(CARDiCommon *p, s32 command, s32 retry_count)
{
    if ((p->flag & 2) == 0) {
        p->flag |= 2;
        while (!func_02027f4c(0xb, 1))
            func_020263c0(100);
        func_0202dc14(p, 0, 1);
    }
    func_02025c94((u32)p->cmd, 0x20);
    func_02025cb0();

    do {
        p->command = command;
        p->flag |= 0x20;
        cardi_send_pxi((u32)command);
        if (command == 0) {
            cardi_send_pxi((u32)p->cmd);
        }
        {
            u32 bak_psr = func_02026350();

            while ((p->flag & 0x20) != 0)
                func_020251c0(0);
            func_02026364(bak_psr);
        }
    } while ((p->cmd->result == 4) && (--retry_count > 0));

    return (p->cmd->result == 0);
}