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
} WMDataSharingInfo;

typedef struct {
    u8 pad0[0x10];
    u32 mp_flag;
    u8 pad1[0x86 - 0x14];
    u16 mp_readyBitmap;
    u8 pad2[0x184 - 0x88];
    u16 aid;
} WMStatus;

typedef struct {
    u8 pad[4];
    WMStatus *status;
} WMArm9Buf;

extern u32 func_0202e2ec(void);
extern u32 func_0202e1cc(s32 paramNum, ...);
extern void func_02025c5c(u32 a0, u32 a1);
extern void func_02026a04(register u32 data, register void *destp, register u32 size);
extern u32 func_0202e9c4(u32 a0, u32 a1, u32 a2);
extern s32 func_0202f118(void (*cb)(void), s32 a1, s32 a2, s32 a3, u16 b0, u16 b1, u16 b2);
extern void func_0202f8c4(void);
extern void func_0202f77c(void);
extern void func_0202fa08(void);

s32 func_0202ffe4(WMDataSharingInfo *dsInfo, u16 port, u16 aidBitmap,
                  u16 dataLength, u32 doubleMode)
{
    s32 result;
    int aid;
    u16 mpReadyBitmap = 0x0001;
    WMArm9Buf *p = (WMArm9Buf *)func_0202e2ec();

    result = func_0202e1cc(2, 9, 10);
    if (result != 0)
        return result;

    func_02025c5c((u32)&p->status->mp_flag, 4);
    if (p->status->mp_flag == 0)
        return 3;

    if (dsInfo == 0)
        return 6;

    if (port >= 0x10)
        return 6;

    if (aidBitmap == 0)
        return 6;

    func_02025c5c((u32)&p->status->aid, 2);
    aid = p->status->aid;

    if (aid == 0) {
        func_02025c5c((u32)&p->status->mp_readyBitmap, 2);
        mpReadyBitmap = p->status->mp_readyBitmap;
    }

    {
        volatile u32 zero = 0;
        func_02026a04(zero, dsInfo, sizeof(WMDataSharingInfo));
    }

    dsInfo->writeIndex = 0;
    dsInfo->sendIndex = 0;
    dsInfo->readIndex = 0;
    dsInfo->dataLength = dataLength;
    dsInfo->port = port;
    dsInfo->aidBitmap = 0;
    dsInfo->doubleMode = (u16)((doubleMode) ? 1 : 0);

    aidBitmap |= (1 << aid);
    dsInfo->aidBitmap = aidBitmap;

    {
        u16 count = 0;
        u16 iAid;
        u16 mask = 0x0001;

        for (iAid = 0; iAid < 16; iAid++) {
            if (aidBitmap & mask)
                count++;
            mask <<= 1;
        }

        dsInfo->stationNumber = count;
        dsInfo->dataSetLength = (u16)(dataLength * count);

        if (dsInfo->dataSetLength > 0x1fc) {
            dsInfo->aidBitmap = 0;
            return 6;
        }

        dsInfo->dataSetLength += 4;
    }

    dsInfo->state = 1;

    if (aid == 0) {
        int i;

        for (i = 0; i < 4; i++)
            dsInfo->ds[i].aidBitmap = (u16)(dsInfo->aidBitmap & (mpReadyBitmap | 0x0001));

        func_0202e9c4(port, (u32)func_0202f8c4, (u32)dsInfo);

        for (i = 0; i < ((dsInfo->doubleMode == 1) ? 2 : 1); i++) {
            s32 res;

            dsInfo->writeIndex = (u16)((dsInfo->writeIndex + 1) % 4);

            res = func_0202f118(func_0202fa08, (s32)dsInfo,
                                (s32)&dsInfo->ds[i], dsInfo->dataSetLength,
                                (u16)(dsInfo->aidBitmap & mpReadyBitmap),
                                dsInfo->port, 1);

            if (res == 7) {
                dsInfo->seqNum[i] = 0xffff;
                dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
            } else {
                if (res != 0 && res != 2) {
                    dsInfo->state = 5;
                    return 1;
                }
            }
        }
    } else {
        dsInfo->sendIndex = (u16)(4 - 1);
        func_0202e9c4(port, (u32)func_0202f77c, (u32)dsInfo);
    }

    return 0;
}