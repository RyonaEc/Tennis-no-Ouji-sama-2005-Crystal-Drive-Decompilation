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
extern void func_020269d0(u16 *src, u16 *dst, s32 count);
extern s32 func_0202f118(void (*cb)(void), s32 a1, s32 a2, s32 a3, u16 b0, u16 b1, u16 b2);
extern void func_0202f4b8(WMDataSharingInfo *dsInfo, s32 delayed);
extern void func_0202f668(WMDataSharingInfo *dsInfo, u16 aid, u16 *data);
extern void func_0202fa08(void);

s32 func_0202fb2c(WMDataSharingInfo *dsInfo, const u16 *sendData, WMDataSet *receiveData)
{
    s32 result;
    u16 aid;
    u16 mpReadyBitmap;
    WMArm9Buf *p = (WMArm9Buf *)func_0202e2ec();
    u16 state;

    result = func_0202e1cc(2, 9, 10);
    if (result != 0)
        return result;

    func_02025c5c((u32)&p->status->mp_flag, 4);
    if (p->status->mp_flag == 0)
        return 3;

    if (dsInfo == 0)
        return 6;

    if (sendData == 0)
        return 6;

    if (receiveData == 0)
        return 6;

    func_02025c5c((u32)&p->status->aid, 2);
    aid = p->status->aid;

    if (aid == 0) {
        func_02025c5c((u32)&p->status->mp_readyBitmap, 2);
        mpReadyBitmap = p->status->mp_readyBitmap;
    }

    state = dsInfo->state;
    if (state == 5)
        return 1;

    if (state != 1 && state != 4)
        return 3;

    result = 5;

    if (aid == 0) {
        s32 sendFlag = 0;
        s32 delayed = 0;

        if (state == 4) {
            s32 res;
            s32 oldWI;

            dsInfo->state = 1;
            oldWI = (u16)((dsInfo->writeIndex + 4 - 1) % 4);
            res = func_0202f118(func_0202fa08, (s32)dsInfo,
                                (s32)&dsInfo->ds[oldWI], dsInfo->dataSetLength,
                                (u16)(dsInfo->aidBitmap & mpReadyBitmap),
                                dsInfo->port, 1);
            if (res == 7) {
                dsInfo->seqNum[oldWI] = 0xffff;
                dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
            } else {
                if (res != 0 && res != 2) {
                    dsInfo->state = 5;
                    return 1;
                }
            }
        }

        if (dsInfo->readIndex != dsInfo->sendIndex) {
            dsInfo->ds[dsInfo->readIndex].aidBitmap |= 0x0001;
            func_020269d0((u16 *)&dsInfo->ds[dsInfo->readIndex], (u16 *)receiveData,
                          sizeof(WMDataSet));
            dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
            dsInfo->readIndex = (u16)((dsInfo->readIndex + 1) % 4);

            sendFlag = 1;
            result = 0;

            if (dsInfo->doubleMode == 0 && mpReadyBitmap != 0
                && dsInfo->ds[dsInfo->writeIndex].aidBitmap == 0x0001) {
                delayed = 1;
            } else {
                delayed = 0;
            }
        }

        func_0202f4b8(dsInfo, 0);

        if (sendFlag) {
            func_0202f668(dsInfo, 0, (u16 *)sendData);

            if (dsInfo->doubleMode == 0)
                func_0202f4b8(dsInfo, delayed);
        }
    } else {
        s32 sendFlag = 0;

        if (state == 4) {
            sendFlag = 1;
            dsInfo->state = 1;
        } else {
            if (dsInfo->readIndex != dsInfo->writeIndex) {
                if (!(dsInfo->ds[dsInfo->readIndex].aidBitmap & 0x0001)) {
                    dsInfo->ds[dsInfo->readIndex].aidBitmap |= 0x0001;
                } else {
                    func_020269d0((u16 *)&dsInfo->ds[dsInfo->readIndex],
                                  (u16 *)receiveData, sizeof(WMDataSet));
                    dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
                    dsInfo->readIndex = (u16)((dsInfo->readIndex + 1) % 4);

                    sendFlag = 1;
                    result = 0;
                }
            }
        }

        if (sendFlag) {
            s32 res;
            u16 *buf = (u16 *)(((u8 *)&dsInfo->ds[dsInfo->sendIndex]) + 32);

            func_020269d0((u16 *)sendData, buf, dsInfo->dataLength);
            res = func_0202f118(func_0202fa08, (s32)dsInfo, (s32)buf,
                                dsInfo->dataLength, dsInfo->aidBitmap,
                                dsInfo->port, 1);
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
            if (res != 2 && res != 0) {
                dsInfo->state = 5;
                result = 1;
            }
        }
    }

    return result;
}