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

extern u32 func_02026350(void);
extern void func_02026364(u32 a);
extern void func_0202f4b8(WMDSInfo *dsInfo, s32 delayed);
extern void func_0202f668(WMDSInfo *dsInfo, u16 aid, u16 *data);

void func_0202f8c4(void *callback)
{
    WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback;
    WMDSInfo *dsInfo = (WMDSInfo *)cb_Port->arg;

    if (dsInfo == 0)
        return;

    if (cb_Port->errcode == 0) {
        switch (cb_Port->state) {
        case 21:
            func_0202f668(dsInfo, cb_Port->aid, cb_Port->data);
            func_0202f4b8(dsInfo, 0);
            break;
        case 7:
            func_0202f4b8(dsInfo, 0);
            break;
        case 9: {
            u16 aidBit;
            u32 enabled;

            aidBit = (u16)(1 << cb_Port->aid);
            enabled = func_02026350();
            dsInfo->ds[dsInfo->writeIndex].aidBitmap &= ~aidBit;
            if (dsInfo->doubleMode == 1)
                dsInfo->ds[(dsInfo->writeIndex + 1) % 4].aidBitmap &= ~aidBit;
            func_02026364(enabled);
            func_0202f4b8(dsInfo, 0);
            if (dsInfo->doubleMode == 1)
                func_0202f4b8(dsInfo, 0);
            break;
        }
        }
    } else {
        dsInfo->state = 5;
    }
}