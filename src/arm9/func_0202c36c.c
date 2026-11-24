#include "types.h"

typedef struct {
    u32 lock;
    void (*callback)(u32, void *);
    void *callbackArg;
} PMWork;

extern PMWork data_0204acb8;

void func_0202c36c(u32 result)
{
    void (*callback)(u32, void *) = data_0204acb8.callback;
    void *arg = data_0204acb8.callbackArg;

    if (data_0204acb8.lock)
        data_0204acb8.lock = 0;

    if (callback) {
        data_0204acb8.callback = 0;
        callback(result, arg);
    }
}