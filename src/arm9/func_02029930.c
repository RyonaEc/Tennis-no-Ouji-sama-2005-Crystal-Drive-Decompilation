#include "types.h"

typedef struct {
    u32 a;
    u32 b;
} P;

extern s32 func_02029a5c(P *);
extern s32 func_02029980(s32, P);

s32 func_02029930(s32 a0)
{
    P p;

    if (func_02029a5c(&p) != 0) {
        if (func_02029980(a0, p) != 0)
            return 1;
    }
    return 0;
}