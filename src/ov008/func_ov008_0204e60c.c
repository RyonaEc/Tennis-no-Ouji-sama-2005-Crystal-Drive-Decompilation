#include "types.h"

extern u32 func_ov008_0204ed28(void);
extern void func_ov008_0204cdac(void);
extern void func_ov008_0204ca78(u8 *p);

typedef struct {
    u32 a;
    u16 pad;
    u16 b;
} Rec;

void func_ov008_0204e60c(void)
{
    Rec *p = (Rec *)func_ov008_0204ed28();
    u16 i;

    for (i = 0; i < 0x80; i++) {
        p[i].a = 0x200;
        p[i].b = 0;
    }
    func_ov008_0204cdac();
    func_ov008_0204ca78((u8 *)p);
}