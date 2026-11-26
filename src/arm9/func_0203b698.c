#include "types.h"

typedef void (*Fn)(void);

extern Fn ARM9_CTOR_START[];

void func_0203b698(void)
{
    Fn *p = ARM9_CTOR_START;
    Fn f;

    while (p != 0 && (f = *p) != 0) {
        f();
        p++;
    }
}