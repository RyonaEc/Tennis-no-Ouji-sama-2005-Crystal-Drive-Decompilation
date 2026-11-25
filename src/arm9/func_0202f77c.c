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
    u16 apiid;
    u16 errcode;
    u16 state;
    u16 port;
    u32 recvBuf;
    u16 *data;
    u16 length;
    u16 aid;
    u8 macAddress[6];
    u16 seqNo;
    void *arg;
} WMPortRecvCallback;

typedef struct {
    u8 pad[0x184];
    u16 aid;
} WMStatus;

typedef struct {
    u8 pad[4];
    WMStatus *status;
} WMArm9Buf;

extern u32 func_0202e2ec(void);
extern void func_02025c5c(u16 *p, u32 n);
extern void func_020269d0(u16 *src, u16 *dst, s32 count);

void func_0202f77c(void *callback)
{
    WMArm9Buf *p = (WMArm9Buf *)func_0202e2ec();
    WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback;
    WMDSInfo *dsInfo = (WMDSInfo *)cb_Port->arg;
    u16 length;

    if (dsInfo == 0)
        return;

    if (cb_Port->errcode == 0) {
        switch (cb_Port->state) {
        case 7:
            break;
        case 9:
            break;
        case 21:
            length = cb_Port->length;
            if (length != dsInfo->dataSetLength) {
                if (length > sizeof(WMDataSet))
                    length = sizeof(WMDataSet);
            }
            func_02025c5c(&p->status->aid, 2);
            if (length >= 4 && (((WMDataSet *)cb_Port->data)->aidBitmap && (1 << p->status->aid))) {
                func_020269d0(cb_Port->data, (u16 *)&dsInfo->ds[dsInfo->writeIndex], length);
                dsInfo->seqNum[dsInfo->writeIndex] = (u16)(cb_Port->seqNo >> 1);
                dsInfo->writeIndex = (u16)((dsInfo->writeIndex + 1) % 4);
            }
            break;
        }
    } else {
        dsInfo->state = 5;
    }
}