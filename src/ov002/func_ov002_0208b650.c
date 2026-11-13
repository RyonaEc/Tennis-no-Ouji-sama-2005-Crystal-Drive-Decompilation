#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_ov002_0208b3e8(void);
extern void func_ov002_0208a3cc(void);
extern void func_ov002_0208ac0c(u32, u32, u32);
extern void func_ov002_02088948(void);
void func_ov002_0208b650(void)
{
    u8 *b = B;
    *(u32 *)(b + 0x4000 + 0xce8) = 0;
    func_ov002_0208b3e8();
    func_ov002_0208a3cc();
    func_ov002_0208ac0c(0, 0, 0);
    func_ov002_02088948();
}
