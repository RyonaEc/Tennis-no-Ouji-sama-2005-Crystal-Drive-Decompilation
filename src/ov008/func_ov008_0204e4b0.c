#include "types.h"

extern u8 data_ov008_0204f440[];

extern void func_ov008_0204ed28(void);
extern void func_ov008_0204e554(void);
extern void func_ov008_0204df5c(void);
extern void func_ov008_0204c9ac(void);
extern void func_ov008_0204e434(void);
extern void func_ov008_0204e75c(u32 a0, u32 a1);
extern void func_ov008_0204c8dc(u32 a0);
extern void func_ov008_0204e3fc(void);
extern void func_ov008_0204c894(u32 a0);
extern void func_ov008_0204c810(void);
extern void func_ov008_0204ebc8(void);

void func_ov008_0204e4b0(void)
{
    u8 *b = *(u8 **)data_ov008_0204f440;

    func_ov008_0204ed28();
    func_ov008_0204e554();
    func_ov008_0204df5c();
    func_ov008_0204c9ac();
    func_ov008_0204e434();
    func_ov008_0204e75c(0, 0x80);
    b[8] = 0;
    func_ov008_0204c8dc(1);
    func_ov008_0204e3fc();
    func_ov008_0204c894(1);
    func_ov008_0204c810();
    func_ov008_0204ebc8();
    *(u32 *)0x04000540 = 1;
}