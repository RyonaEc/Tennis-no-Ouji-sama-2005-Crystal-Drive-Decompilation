#include "types.h"

typedef signed int s32;
typedef signed short s16;
typedef unsigned int u32;

extern s32 func_0201fdf0(s32 a, s32 b);
extern s16 data_02041908[];

s16 func_02020044(s32 y, s32 x)
{
    s32 a, b, c;
    int sgn;

    if (y > 0) {
        if (x > 0) {
            if (x > y) {
                a = y;
                b = x;
                c = 0;
                sgn = 1;
            } else if (x < y) {
                a = x;
                b = y;
                c = 6434;
                sgn = 0;
            } else {
                return 3217;
            }
        } else if (x < 0) {
            x = -x;
            if (x < y) {
                a = x;
                b = y;
                c = 6434;
                sgn = 1;
            } else if (x > y) {
                a = y;
                b = x;
                c = 12868;
                sgn = 0;
            } else {
                return 9651;
            }
        } else {
            return 6434;
        }
    } else if (y < 0) {
        y = -y;
        if (x < 0) {
            x = -x;
            if (x > y) {
                a = y;
                b = x;
                c = -12868;
                sgn = 1;
            } else if (x < y) {
                a = x;
                b = y;
                c = -6434;
                sgn = 0;
            } else {
                return -9651;
            }
        } else if (x > 0) {
            if (x < y) {
                a = x;
                b = y;
                c = -6434;
                sgn = 1;
            } else if (x > y) {
                a = y;
                b = x;
                c = 0;
                sgn = 0;
            } else {
                return -3217;
            }
        } else {
            return -6434;
        }
    } else {
        if (x >= 0)
            return 0;
        else
            return 12868;
    }

    if (b == 0)
        return 0;
    if (sgn)
        return (s16)(c + data_02041908[func_0201fdf0(a, b) >> 5]);
    else
        return (s16)(c - data_02041908[func_0201fdf0(a, b) >> 5]);
}