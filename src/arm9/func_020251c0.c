#include "types.h"

typedef struct Obj Obj;
struct Obj { u8 pad[0x64]; u32 f64; u8 pad2[4]; u32 f6c; };

extern u32 data_02048d34;
extern u32 func_02026350(void);
extern void func_02026364(u32);
extern void func_02025024(void);

void func_020251c0(u16 *p)
{
    u32 tok = func_02026350();
    Obj *o = *(Obj **)data_02048d34;

    if (p != 0)
        *p |= (u16)(1 << o->f6c);
    o->f64 = 0;
    func_02025024();
    func_02026364(tok);
}