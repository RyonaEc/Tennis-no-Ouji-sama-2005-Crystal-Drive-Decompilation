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

extern u32 func_0202f404(u8 *p, u32 aidBitmap, s32 data, s32 aid);
extern void func_020269d0(u8 *src, u8 *dst, s32 count);
extern void func_020269b8(u16 v, u8 *dst, s32 count);
extern u32 func_02026350(void);
extern void func_02026364(u32 a);

void func_0202f668(WMDSInfo *dsInfo, u16 aid, u16 *data)
{
    u16 aidBit = (u16)(1 << aid);
    u16 writeIndex;
    u8 *buf;
    u32 enabled;

    if (!(dsInfo->aidBitmap & aidBit))
        return;

    if (!(dsInfo->ds[dsInfo->writeIndex].aidBitmap & aidBit)) {
        if (dsInfo->doubleMode != 1)
            return;
        writeIndex = (u16)((dsInfo->writeIndex + 1) % 4);
        if (!(dsInfo->ds[writeIndex].aidBitmap & aidBit))
            return;
    } else {
        writeIndex = dsInfo->writeIndex;
    }

    buf = (u8 *)func_0202f404((u8 *)dsInfo, dsInfo->aidBitmap,
                             (s32)&dsInfo->ds[writeIndex].data[0], aid);

    if (data != 0) {
        func_020269d0((u8 *)data, buf, dsInfo->dataLength);
    } else {
        volatile u16 z = 0;

        func_020269b8(z, buf, dsInfo->dataLength);
    }

    enabled = func_02026350();
    dsInfo->ds[writeIndex].aidBitmap &= ~aidBit;
    dsInfo->ds[writeIndex].receivedBitmap |= aidBit;
    func_02026364(enabled);
}