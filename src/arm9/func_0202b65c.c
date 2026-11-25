#include "types.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u16 x;
    u16 y;
    u16 touch;
    u16 validity;
} TPData;

typedef union {
    u16 halfs[2];
    struct {
        u32 x : 12;
        u32 y : 12;
        u32 touch : 1;
        u32 validity : 2;
    } e;
} SPITpData;

extern struct TPSt data_0204ac68;

struct TPSt {
    void (*cb)(s32, s32, u32);
    TPData buf;
    u16 index;
    u16 frequence;
    TPData *samplingBufs;
    u16 bufSize;
    u8 pad2[2];
    s32 calibrate[6];
    u16 calibrate_flg;
    u16 state;
    volatile u16 err_flg;
    volatile u16 command_flg;
};

extern void func_020263cc(void);

#define TP_COPY(dst, tmp) do { \
    SPITpData tmp; \
    tmp.halfs[0] = *(u16 *)0x027fffaa; \
    tmp.halfs[1] = *(u16 *)0x027fffac; \
    (dst)->x = (u16)tmp.e.x; \
    (dst)->y = (u16)tmp.e.y; \
    (dst)->touch = (u8)tmp.e.touch; \
    (dst)->validity = (u8)tmp.e.validity; \
} while (0)

void func_0202b65c(s32 tag, u32 data, s32 err)
{
    u16 result;
    u16 command;
    TPData *p;

    result = (u16)(data & 0xffff);
    command = (u16)((result & 0x7f00) >> 8);

    if (err) {
        data_0204ac68.err_flg |= (1 << command);
        if (data_0204ac68.cb != 0)
            data_0204ac68.cb(command, 4, 0);
        return;
    }

    if (command == 0x10) {
        data_0204ac68.index++;
        if (data_0204ac68.index >= data_0204ac68.bufSize)
            data_0204ac68.index = 0;

        p = &data_0204ac68.samplingBufs[data_0204ac68.index];
        TP_COPY(p, spi_tp1);

        if (data_0204ac68.cb != 0)
            data_0204ac68.cb(command, 0, data_0204ac68.index & 0xff);
        return;
    }

    if (!(data & 0x01000000))
        return;

    switch (result & 0xff) {
    case 0:
        switch (command) {
        case 0:
            TP_COPY(&data_0204ac68.buf, spi_tp2);
            data_0204ac68.state = 0;
            break;
        case 1:
            data_0204ac68.state = 2;
            break;
        case 2:
            data_0204ac68.state = 0;
            break;
        }
        data_0204ac68.command_flg &= ~(1 << command);
        if (data_0204ac68.cb != 0)
            data_0204ac68.cb(command, 0, 0);
        break;

    case 4:
        result = 3;
        goto common;
    case 2:
        result = 1;
        goto common;
    case 3:
        result = 2;

common:
        data_0204ac68.err_flg |= (1 << command);
        data_0204ac68.command_flg &= ~(1 << command);
        if (data_0204ac68.cb != 0)
            data_0204ac68.cb(command, result & 0xff, 0);
        break;

    default:
        func_020263cc();
        return;
    }
}
