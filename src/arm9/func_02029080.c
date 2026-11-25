#include "types.h"

typedef struct {
    u32 arc;
    u16 own_id;
    u16 index;
    u32 pos;
} FSDirPos;

extern u8 *data_0204abe8;
extern FSDirPos data_0204abec;
extern u32 func_02026350(void);
extern void func_02026364(u32 bak);
extern u8 *func_0202963c(u8 *name, u32 len);
extern u32 func_02029704(u8 *name, u32 len);

u32 func_02029080(u8 *p_arc, u8 *name, u32 len)
{
    u32 bak;
    u32 result = 0;
    u8 *pos;

    bak = func_02026350();

    if (func_0202963c(name, len) == 0) {
        if (data_0204abe8 == 0) {
            data_0204abe8 = p_arc;
            data_0204abec.arc = (u32)p_arc;
            data_0204abec.pos = 0;
            data_0204abec.index = 0;
            data_0204abec.own_id = 0;
        } else {
            pos = data_0204abe8;
            while (*(u8 **)(pos + 4) != 0)
                pos = *(u8 **)(pos + 4);
            *(u8 **)(pos + 4) = p_arc;
            *(u8 **)(p_arc + 8) = pos;
        }

        *(u32 *)p_arc = func_02029704(name, len);
        result = 1;
    }

    func_02026364(bak);

    return result;
}