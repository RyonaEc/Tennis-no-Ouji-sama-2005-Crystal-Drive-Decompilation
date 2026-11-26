#include "types.h"

typedef struct {
    u8 pad0[0x10];
    u16 mode;
    u8 pad1[0x58 - 0x12];
    u16 f58;
    u16 f5a;
    u16 f5c;
    u16 f5e;
    u16 f60;
    u16 f62;
} OV003State;

typedef struct {
    u8 info[8];
    s32 top;
    s32 bottom;
} PanelBuf;

extern u8 data_ov003_020eb720[];

extern void func_02010358(u8 *a0);
extern s32 func_0202bda0(s32 *top, s32 *bottom);

void func_ov003_020d29d0(void)
{
    OV003State *state = (OV003State *)(*(u32 *)data_ov003_020eb720 + 0xb8dc);
    PanelBuf buf;

    func_02010358(buf.info);

    state->f58 = buf.info[0];
    state->f5a = buf.info[1];

    if (buf.info[4] != 0)
        state->f60 = 0;
    else
        state->f60 = 1;

    if (buf.info[2] != 0)
        state->f5c = 0;
    else
        state->f5c = 1;

    state->f5e = buf.info[3] - 1;

    if (func_0202bda0(&buf.top, &buf.bottom) == 0) {
        if (buf.top == 1) {
            state->f62 = 0;
            state->mode = state->f62;
        } else {
            state->f62 = 1;
            state->mode = state->f62;
        }
        return;
    }

    state->f62 = 0;
}