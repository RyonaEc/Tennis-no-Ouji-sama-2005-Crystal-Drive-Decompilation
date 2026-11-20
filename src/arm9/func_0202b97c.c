#include "types.h"

typedef struct N {
    void (*fn)(u32);
    u32 arg;
    struct N *next;
} N;

void func_0202b97c(N *n) {
    if (n == 0)
        return;
    do {
        n->fn(n->arg);
        n = n->next;
    } while (n != 0);
}