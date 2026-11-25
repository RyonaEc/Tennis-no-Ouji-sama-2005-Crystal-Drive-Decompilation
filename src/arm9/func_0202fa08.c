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
    u16 pad6;
    u16 pad8;
    u16 port;
    u16 padc;
    u16 pade;
    u32 pad10;
    u32 pad14;
    u32 arg;
    u16 seqNo;
} WMPortSendCallback;

typedef struct {
    u8 pad[0x184];
    u16 aid;
} WMStatus;

typedef void (*WMCb)(void *);

typedef struct {
    u8 pad[4];
    WMStatus *status;
    u8 pad2[0xb0];
    WMCb portCallbackTable[16];
    WMDSInfo *portCallbackArgument[16];
} WMArm9Buf;

extern u32 func_0202e2ec(void);
extern void func_02025c5c(u16 *p, u32 n);
extern void func_0202f8c4(void *a);
extern void func_0202f77c(void *a);

void func_0202fa08(void *callback)
{
    WMArm9Buf *p = (WMArm9Buf *)func_0202e2ec();
    WMPortSendCallback *cb_Port = (WMPortSendCallback *)callback;
    WMDSInfo *dsInfo;
    u16 aid;

    dsInfo = p->portCallbackArgument[cb_Port->port];
    if ((p->portCallbackTable[cb_Port->port] != (WMCb)func_0202f8c4
         && p->portCallbackTable[cb_Port->port] != (WMCb)func_0202f77c)
        || dsInfo == 0
        || dsInfo != (WMDSInfo *)cb_Port->arg)
        return;

    func_02025c5c(&p->status->aid, 2);
    aid = p->status->aid;

    if (cb_Port->errcode == 0) {
        if (aid == 0) {
            dsInfo->seqNum[dsInfo->sendIndex] = (u16)(cb_Port->seqNo >> 1);
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
        }
    } else if (cb_Port->errcode == 10) {
        if (aid != 0)
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 4 - 1) % 4);
        dsInfo->state = 4;
    } else {
        dsInfo->state = 5;
    }
}