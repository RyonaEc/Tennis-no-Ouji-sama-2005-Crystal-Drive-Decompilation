#include "types.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u16 aidBitmap;
    u16 receivedBitmap;
    u16 data[254];
} WMDataSet;

typedef struct {
    WMDataSet ds[4];
    u16 seqNum[4];
    u16 writeIndex;
    u16 sendIndex;
    u16 readIndex;
    u16 aidBitmap;
    u16 dataLength;
    u16 stationNumber;
    u16 dataSetLength;
    u16 port;
    u16 doubleMode;
    u16 currentSeqNum;
    u16 state;
    u16 reserved[1];
} WMDSInfo;

typedef struct {
    u8 pad[0x86];
    u16 mpReadyBitmap;
} WMStatus;

typedef struct {
    u8 pad[4];
    WMStatus *status;
} WMArm9Buf;

extern u32 func_0202e2ec(void);
extern u32 func_02026350(void);
extern void func_02026364(u32 a);
extern void func_02025c5c(u32 a0, u32 a1);
extern void func_020269b8(u16 v, u8 *dst, s32 count);
extern s32 func_0202f118(s32 a0, s32 a1, s32 a2, s32 a3, u16 b0, u16 b1, u16 b2);
extern void func_0202fa08(void);

void func_0202f4b8(WMDSInfo *dsInfo, s32 delayed)
{
    WMArm9Buf *p = (WMArm9Buf *)func_0202e2ec();
    u32 enabled;
    s32 res;
    u16 newWI, oldWI, resetWI;
    u16 mpReadyBitmap;

    enabled = func_02026350();

    if (dsInfo->ds[dsInfo->writeIndex].aidBitmap == 0) {
        func_02025c5c((u32)&p->status->mpReadyBitmap, 2);
        mpReadyBitmap = p->status->mpReadyBitmap;

        oldWI = dsInfo->writeIndex;
        newWI = (u16)((oldWI + 1) % 4);

        if (dsInfo->doubleMode == 1)
            resetWI = (u16)((newWI + 1) % 4);
        else
            resetWI = newWI;

        {
            volatile u16 zero = 0;
            func_020269b8(zero, (u8 *)&dsInfo->ds[resetWI], sizeof(WMDataSet));
        }

        dsInfo->ds[resetWI].aidBitmap = (u16)(dsInfo->aidBitmap & (mpReadyBitmap | 0x0001));
        dsInfo->writeIndex = newWI;
        dsInfo->ds[oldWI].aidBitmap = dsInfo->aidBitmap;

        if (delayed == 1)
            dsInfo->ds[oldWI].aidBitmap &= ~0x0001;

        func_02026364(enabled);

        res = func_0202f118((s32)func_0202fa08, (s32)dsInfo,
                            (s32)&dsInfo->ds[oldWI], dsInfo->dataSetLength,
                            (u16)(dsInfo->aidBitmap & mpReadyBitmap),
                            dsInfo->port, 1);

        if (res == 7) {
            dsInfo->seqNum[oldWI] = 0xffff;
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
        } else {
            if (res != 0 && res != 2)
                dsInfo->state = 5;
        }
    } else {
        func_02026364(enabled);
    }
}