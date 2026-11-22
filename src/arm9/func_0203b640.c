#include "types.h"

typedef struct N N;
struct N { N *next; void (*cb)(u32, s32); u32 arg; };

extern N *data_0204c790;

void func_0203b640(void)
{
    N *n = data_0204c790;

    while (n != 0)
    {
        data_0204c790 = n->next;
        n->cb(n->arg, -1);
        n = data_0204c790;
    }
}