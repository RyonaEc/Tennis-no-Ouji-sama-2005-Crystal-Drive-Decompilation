#include "types.h"

typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u32 dunno[3];
    u32 src;
    u32 dst;
    u32 len;
} CARDiCmd;

typedef struct {
    CARDiCmd *cmd;
    u8 pad1[0x14];
    u32 src;
    u32 dst;
    u32 len;
    u8 pad2[0xe0 - 0x24];
    u8 backup_cache_page_buf[0x100];
} CARDiCommon;

extern void func_02026b88(register const void *srcp, register void *dstp, register u32 size);
extern void func_02025c94(u32 a0, u32 a1);
extern void func_02025cb0(void);
extern u32 func_0202dc14(u32 p, u32 req, u32 retry);
extern void func_0202d4f0(u8 *p);

void func_0202d300(CARDiCommon *p)
{
    do {
        const u32 size = sizeof(p->backup_cache_page_buf);
        const u32 len = (size < p->len) ? size : p->len;

        func_02026b88((const void *)p->src, p->backup_cache_page_buf, len);
        func_02025c94((u32)p->backup_cache_page_buf, len);
        func_02025cb0();

        p->cmd->src = (u32)p->backup_cache_page_buf;
        p->cmd->dst = (u32)p->dst;
        p->cmd->len = len;

        if (!func_0202dc14((u32)p, 8, 10))
            break;

        if (!func_0202dc14((u32)p, 9, 1))
            break;

        p->src += len;
        p->dst += len;
        p->len -= len;
    } while (p->len > 0);

    func_0202d4f0((u8 *)p);
}