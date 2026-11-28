#include "types.h"

typedef union {
    struct {
        u32 tag : 5;
        u32 err : 1;
        u32 data : 26;
    } e;
    u32 raw;
} PXIFifoMessage;

typedef void (*PXIFifoCallback)(u32 tag, u32 data, u32 err);

extern u32 data_0204ab68[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);

#define REG_PXI_FIFO_CNT (*(volatile u16 *)0x04000184)
#define REG_PXI_RECV_FIFO (*(volatile u32 *)0x04100000)
#define REG_PXI_SEND_FIFO (*(volatile u32 *)0x04000188)

static inline s32 PXIi_GetFromFifo(u32 *data_buf)
{
    u32 enabled;

    if (REG_PXI_FIFO_CNT & 0x4000) {
        REG_PXI_FIFO_CNT |= 0xc000;
        return -3;
    }
    enabled = func_02026350();
    if (REG_PXI_FIFO_CNT & 0x100) {
        func_02026364(enabled);
        return -4;
    }
    *data_buf = REG_PXI_RECV_FIFO;
    func_02026364(enabled);
    return 0;
}

static inline s32 PXIi_SetToFifo(u32 data)
{
    u32 enabled;

    if (REG_PXI_FIFO_CNT & 0x4000) {
        REG_PXI_FIFO_CNT |= 0xc000;
        return -3;
    }
    enabled = func_02026350();
    if (REG_PXI_FIFO_CNT & 0x2) {
        func_02026364(enabled);
        return -2;
    }
    REG_PXI_SEND_FIFO = data;
    func_02026364(enabled);
    return 0;
}

void func_02027d74(void)
{
    PXIFifoMessage fifomsg;
    s32 ret_code;
    u32 tag;

    for (;;) {
        ret_code = PXIi_GetFromFifo(&fifomsg.raw);
        if (ret_code == -4) {
            break;
        }
        if (ret_code == -3) {
            continue;
        }
        tag = fifomsg.e.tag;
        if (tag) {
            if (data_0204ab68[tag]) {
                ((PXIFifoCallback)data_0204ab68[tag])(tag, fifomsg.e.data, (u32)fifomsg.e.err);
            } else {
                if (fifomsg.e.err) {
                } else {
                    fifomsg.e.err = 1;
                    PXIi_SetToFifo(fifomsg.raw);
                }
            }
        } else {
        }
    }
}