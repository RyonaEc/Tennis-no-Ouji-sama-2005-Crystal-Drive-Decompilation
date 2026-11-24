#include "types.h"

extern u32 func_0202c090(u32 number, u32 callback, void *arg);

u32 func_0202bea8(u32 target, u32 sw, u32 callback, void *arg)
{
    u32 command = 0;

    if (target == 0) {
        if (sw == 1)
            command = 6;
        if (sw == 0)
            command = 7;
    } else if (target == 1) {
        if (sw == 1)
            command = 4;
        if (sw == 0)
            command = 5;
    } else if (target == 2) {
        if (sw == 1)
            command = 8;
        if (sw == 0)
            command = 9;
    }

    return (command) ? func_0202c090(command, callback, arg) : 0xffff;
}