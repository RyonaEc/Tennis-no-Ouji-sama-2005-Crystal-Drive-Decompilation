#include "types.h"

typedef struct Unk02025b88 {
    u8 pad0[4];
    u32 f4;
    u32 f8;
} Unk02025b88;

typedef struct Unk02025b88Head {
    u8 pad[0x78];
    Unk02025b88 *f78;
} Unk02025b88Head;

extern u32 data_02048d3c[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_02025a2c(u32 a0, Unk02025b88 *a1);
extern void func_020251c0(Unk02025b88 *p);

void func_02025b88(Unk02025b88 *p)
{
    u32 irq = func_02026350();
    Unk02025b88Head *head = (Unk02025b88Head *)data_02048d3c[2];

    for (;;) {
        if (p->f4 == 0) {
            p->f4 = (u32)head;
            p->f8++;
            func_02025a2c((u32)head, p);
            break;
        }
        if (p->f4 == (u32)head) {
            p->f8++;
            break;
        }
        head->f78 = p;
        func_020251c0(p);
        head->f78 = 0;
    }

    func_02026364(irq);
}