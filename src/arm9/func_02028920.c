#include "types.h"

typedef s32 FSResult;
typedef struct FSArchive FSArchive;

struct FSArchive {
    u32 pad[16];
    FSResult (*read_func)(FSArchive *, void *, u32, u32);
};

typedef struct {
    u32 pad0[2];
    FSArchive *arc;
    u32 pad1[7];
    u32 pos;
    void *asc;
    u32 pad2;
    u32 len;
} FSFile;

FSResult func_02028920(FSFile *p_file)
{
    FSArchive * const p_arc = p_file->arc;
    const u32 pos = p_file->pos;
    const u32 len = p_file->len;
    void * const dst = p_file->asc;
    p_file->pos += len;
    return (*p_arc->read_func)(p_arc, dst, pos, len);
}