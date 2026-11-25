#include "types.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    char pad[0x80e];
    u16 aidBitmap;
    char pad2[6];
    u16 port;
    char pad3[4];
    u16 state;
} WMDataSharingInfo;

typedef struct {
    char pad[0xb8];
    void (*portCallbackTable[4])(void);
} WMArm9Buf;

extern WMArm9Buf *func_0202e2ec(void);
extern s32 func_0202e1cc(s32 paramNum, ...);
extern u32 func_0202e9c4(u32 a0, u32 a1, u32 a2);
extern void func_0202f8c4(void);
extern void func_0202f77c(void);

s32 func_0202ff28(WMDataSharingInfo *dsInfo)
{
    s32 result;
    WMArm9Buf *p = func_0202e2ec();

    result = func_0202e1cc(2, 9, 10);
    if (result != 0)
        return result;

    if (dsInfo == 0)
        return 6;

    if (dsInfo->aidBitmap == 0
        || (p->portCallbackTable[dsInfo->port] != func_0202f8c4
            && p->portCallbackTable[dsInfo->port] != func_0202f77c))
        return 3;

    func_0202e9c4(dsInfo->port, 0, 0);
    dsInfo->aidBitmap = 0;
    dsInfo->state = 0;

    return 0;
}