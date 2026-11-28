#include "types.h"

typedef struct FSFile FSFile;
typedef struct FSArchive FSArchive;

struct FSFile {
    FSFile *link_prev;      /* 0x00 */
    FSFile *link_next;      /* 0x04 */
    FSArchive *arc;         /* 0x08 */
    u32 stat;               /* 0x0c */
    u32 command;            /* 0x10 */
    u32 error;              /* 0x14 */
    u8 queue[4];            /* 0x18 */
    u8 pad[0x44 - 0x1c];
};

struct FSArchive {
    u32 name;               /* 0x00 */
    FSArchive *next;        /* 0x04 */
    FSArchive *prev;        /* 0x08 */
    u16 sync_q;             /* 0x0c */
    u16 stat_q;             /* 0x0e */
    u32 flag;               /* 0x10 */
    FSFile *list_prev;      /* 0x14 */
    FSFile *list_next;      /* 0x18 */
    u32 base;               /* 0x1c */
    u32 fat;
    u32 fat_size;
    u32 fnt;
    u32 fnt_size;
    u32 fat_bak;
    u32 fnt_bak;
    void *load_mem;         /* 0x38 */
    void *read_func;
    void *write_func;
    void *table_func;
    void (*proc)(FSFile *p_file, u32 command);  /* 0x48 */
    u32 proc_flag;          /* 0x4c */
};

extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_0202514c(u8 *a0);
extern void func_02028c98(FSFile *a0, u32 a1);
extern u32 func_02029c8c(u32 a0);

static inline BOOL fs_arc_canceling(volatile const FSArchive *p) { return (p->flag & 0x20) ? 1 : 0; }
static inline BOOL fs_arc_suspended(volatile const FSArchive *p) { return (p->flag & 0x8) ? 1 : 0; }
static inline BOOL fs_arc_running(volatile const FSArchive *p) { return (p->flag & 0x10) ? 1 : 0; }
static inline BOOL fs_arc_suspending(volatile const FSArchive *p) { return (p->flag & 0x40) ? 1 : 0; }
static inline BOOL fs_file_canceling(volatile const FSFile *p) { return (p->stat & 2) ? 1 : 0; }
static inline BOOL fs_file_syncmode(volatile const FSFile *p) { return (p->stat & 4) ? 1 : 0; }

FSFile *func_02029444(FSArchive *p_arc)
{
    u32 bak_psr = func_02026350();

    if (fs_arc_canceling(p_arc)) {
        FSFile *p, *q;

        p_arc->flag &= ~0x20;
        for (p = p_arc->list_next; p; p = q) {
            q = p->link_next;
            if (fs_file_canceling(p)) {
                if (p_arc->list_next == p)
                    p_arc->list_next = q;
                func_02028c98(p, 3);
                if (!q)
                    q = p_arc->list_next;
            }
        }
    }

    if (!fs_arc_suspended(p_arc) && p_arc->list_next) {
        FSFile *p_file = p_arc->list_next;
        BOOL is_start = !fs_arc_running(p_arc);

        if (is_start)
            p_arc->flag |= 0x10;
        func_02026364(bak_psr);
        if (is_start) {
            if ((p_arc->proc_flag & 0x200) != 0)
                (*p_arc->proc)(p_file, 9);
        }
        if (fs_file_syncmode(p_file)) {
            func_0202514c((u8 *)p_file->queue);
            return 0;
        }
        return p_file;
    }

    if (fs_arc_running(p_arc)) {
        p_arc->flag &= ~0x10;
        if ((p_arc->proc_flag & 0x400) != 0) {
            FSFile tmp;

            func_02029c8c((u32)&tmp);
            tmp.arc = p_arc;
            (*p_arc->proc)(&tmp, 0xa);
        }
    }
    if (fs_arc_suspending(p_arc)) {
        p_arc->flag &= ~0x40;
        p_arc->flag |= 0x8;
        func_0202514c((u8 *)&p_arc->stat_q);
    }
    func_02026364(bak_psr);
    return 0;
}