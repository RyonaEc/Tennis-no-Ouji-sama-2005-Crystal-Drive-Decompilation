#include "types.h"

typedef struct {
    void (*func)(void *);
    u32 enable;
    u32 arg;
} OSIrqCallbackInfo;

extern void *data_02042384[];
extern OSIrqCallbackInfo data_02048ccc[];

void func_02024a54(u32 intrBit, void *function)
{
    int i;
    OSIrqCallbackInfo *info;

    for (i = 0; i < 22; i++) {
        if (intrBit & 1) {
            info = 0;

            if (i >= 8 && i <= 11)
                info = &data_02048ccc[i - 8];
            else if (i >= 3 && i <= 6)
                info = &data_02048ccc[i - 3 + 4];
            else
                data_02042384[i] = function;

            if (info) {
                info->func = function;
                info->arg = 0;
                info->enable = 1;
            }
        }

        intrBit >>= 1;
    }
}