#include "types.h"

typedef struct {
    u8 f0x0;
    u8 f0x1;
    u8 f0x2;
    u8 f0x3;
    u8 f0x4;
    u8 f0x5;
    u8 f0x6;
    u8 f0x7;
    u8 f0x8;
    u8 f0x9;
    u8 f0xa;
    u8 f0xb;
} S_ov002_020855dc;

u32 func_ov002_020855dc(u32 a0, u32 a1)
{
    if (((S_ov002_020855dc *)a0)->f0x0 < ((S_ov002_020855dc *)a1)->f0x0) {
        ((S_ov002_020855dc *)a0)->f0x0 = ((S_ov002_020855dc *)a1)->f0x0;
    }
    if (((S_ov002_020855dc *)a0)->f0x1 < ((S_ov002_020855dc *)a1)->f0x1) {
        ((S_ov002_020855dc *)a0)->f0x1 = ((S_ov002_020855dc *)a1)->f0x1;
    }
    if (((S_ov002_020855dc *)a0)->f0x2 < ((S_ov002_020855dc *)a1)->f0x2) {
        ((S_ov002_020855dc *)a0)->f0x2 = ((S_ov002_020855dc *)a1)->f0x2;
    }
    if (((S_ov002_020855dc *)a0)->f0x3 < ((S_ov002_020855dc *)a1)->f0x3) {
        ((S_ov002_020855dc *)a0)->f0x3 = ((S_ov002_020855dc *)a1)->f0x3;
    }
    if (((S_ov002_020855dc *)a0)->f0x4 < ((S_ov002_020855dc *)a1)->f0x4) {
        ((S_ov002_020855dc *)a0)->f0x4 = ((S_ov002_020855dc *)a1)->f0x4;
    }
    if (((S_ov002_020855dc *)a0)->f0x5 < ((S_ov002_020855dc *)a1)->f0x5) {
        ((S_ov002_020855dc *)a0)->f0x5 = ((S_ov002_020855dc *)a1)->f0x5;
    }
    if (((S_ov002_020855dc *)a0)->f0x6 < ((S_ov002_020855dc *)a1)->f0x6) {
        ((S_ov002_020855dc *)a0)->f0x6 = ((S_ov002_020855dc *)a1)->f0x6;
    }
    if (((S_ov002_020855dc *)a0)->f0x7 < ((S_ov002_020855dc *)a1)->f0x7) {
        ((S_ov002_020855dc *)a0)->f0x7 = ((S_ov002_020855dc *)a1)->f0x7;
    }
    if (((S_ov002_020855dc *)a0)->f0x8 < ((S_ov002_020855dc *)a1)->f0x8) {
        ((S_ov002_020855dc *)a0)->f0x8 = ((S_ov002_020855dc *)a1)->f0x8;
    }
    if (((S_ov002_020855dc *)a0)->f0x9 < ((S_ov002_020855dc *)a1)->f0x9) {
        ((S_ov002_020855dc *)a0)->f0x9 = ((S_ov002_020855dc *)a1)->f0x9;
    }
    if (((S_ov002_020855dc *)a0)->f0xa < ((S_ov002_020855dc *)a1)->f0xa) {
        ((S_ov002_020855dc *)a0)->f0xa = ((S_ov002_020855dc *)a1)->f0xa;
    }
    if (((S_ov002_020855dc *)a0)->f0xb < ((S_ov002_020855dc *)a1)->f0xb) {
        ((S_ov002_020855dc *)a0)->f0xb = ((S_ov002_020855dc *)a1)->f0xb;
    }
    return a0;
}
