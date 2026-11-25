#include "types.h"

typedef struct {
    u32 pad0[3];
    u32 sync_q;
    u32 flag;
    u32 pad1[0xc];
    s32 (*table_func)(void *p_arc, void *dst, u32 pos, u32 len);
} FSArchive;

typedef struct {
    FSArchive *arc;
    u32 pos;
} FSiSyncReadParam;

extern u32 func_02026350(void);
extern void func_020251c0(u32 *p);
extern void func_02026364(u32 bak);

#define IS_SYNC(a) (((a)->flag & 0x200) ? 1 : 0)

void func_02028a38(FSiSyncReadParam *p, void *dst, u32 len)
{
    FSArchive *p_arc = p->arc;

    p_arc->flag |= 0x200;

    switch (p_arc->table_func(p_arc, dst, p->pos, len)) {
    case 0:
    case 1:
        p_arc->flag &= ~0x200;
        break;
    case 6: {
        u32 bak = func_02026350();

        while (IS_SYNC(p_arc) != 0)
            func_020251c0(&p_arc->sync_q);
        func_02026364(bak);
        break;
    }
    }

    p->pos += len;
}