#include "types.h"

typedef struct {
    u32 arc;
    u16 own_id;
    u16 index;
    u32 pos;
} FSDirPos;

extern FSDirPos data_0204abec;
extern u8 *data_0204abe8;
extern u32 func_02026350(void);
extern void func_02026364(u32 bak);

void func_02028fe4(u8 *p_arc)
{
    u32 bak;

    if (*(u32 *)p_arc == 0)
        return;

    bak = func_02026350();

    if (*(u8 **)(p_arc + 4))
        *(u8 **)(*(u8 **)(p_arc + 4) + 8) = *(u8 **)(p_arc + 8);
    if (*(u8 **)(p_arc + 8))
        *(u8 **)(*(u8 **)(p_arc + 8) + 4) = *(u8 **)(p_arc + 4);

    *(u32 *)p_arc = 0;
    *(u8 **)(p_arc + 8) = 0;
    *(u8 **)(p_arc + 4) = *(u8 **)(p_arc + 8);

    *(u32 *)(p_arc + 0x10) &= ~1;

    if (data_0204abec.arc == (u32)p_arc) {
        data_0204abec.arc = (u32)data_0204abe8;
        data_0204abec.pos = 0;
        data_0204abec.index = 0;
        data_0204abec.own_id = 0;
    }

    func_02026364(bak);
}