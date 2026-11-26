#include "types.h"

typedef struct {
    u32 c1;
    u32 c2;
} CTRDGRomCycle;

typedef struct {
    u32 lock;
    u32 proc;
} CTRDGLockByProc;

typedef struct {
    u16 raw;
    u8 pad[3];
    u8 isAgbCartridge : 1;
    u8 detectPullOut : 1;
    u8 unused : 6;
    u8 pad2[2];
    u32 gameCode;
} CTRDGModuleInfo;

extern u8 data_0204c180[];

extern void func_020305ac(u32 lockID, CTRDGLockByProc *info);
extern u32 func_02030584(u32 lockID, CTRDGLockByProc *info);
extern void func_02030660(CTRDGRomCycle *rc);
extern void func_0203062c(CTRDGRomCycle *rc);

s32 func_020306a8(void)
{
    s32 retval = 1;
    CTRDGLockByProc lockInfo;
    CTRDGRomCycle rc;
    u8 isRomCode;
    u8 *cip = (u8 *)0x027ffc30;
    u8 *chp = (u8 *)0x08000000;

    if (*(u16 *)cip == 0xffff)
        return 0;

    if (((CTRDGModuleInfo *)cip)->detectPullOut == 1)
        return 0;

    func_020305ac(*(u16 *)(data_0204c180 + 2), &lockInfo);
    func_02030660(&rc);

    isRomCode = *(u8 *)(chp + 0xb2);

    if ((isRomCode == 0x96 && *(u16 *)cip != *(u16 *)(chp + 0xbe)) ||
        (isRomCode != 0x96 && *(u16 *)cip != *(u16 *)0x0801fffe) ||
        (((CTRDGModuleInfo *)cip)->gameCode != *(u32 *)(chp + 0xac) &&
         ((CTRDGModuleInfo *)cip)->isAgbCartridge)) {
        *(u8 *)(cip + 5) |= 2;
        retval = 0;
    }

    func_0203062c(&rc);
    func_02030584(*(u16 *)(data_0204c180 + 2), &lockInfo);

    return retval;
}