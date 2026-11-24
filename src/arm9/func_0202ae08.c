#include "types.h"

typedef struct {
    u64 div_numer;
    u64 div_denom;
    u64 sqrt;
    u16 div_mode;
    u16 sqrt_mode;
} CPContext;

#define reg_CP_DIV_NUMER    (*(volatile u64 *)0x04000290)
#define reg_CP_DIV_DENOM    (*(volatile u64 *)0x04000298)
#define reg_CP_DIVCNT       (*(volatile u16 *)0x04000280)
#define reg_CP_SQRT_PARAM   (*(volatile u64 *)0x040002b8)
#define reg_CP_SQRTCNT      (*(volatile u16 *)0x040002b0)

void func_0202ae08(CPContext *context)
{
    context->div_numer  = reg_CP_DIV_NUMER;
    context->div_denom  = reg_CP_DIV_DENOM;
    context->div_mode   = (u16) (reg_CP_DIVCNT & 3);
    context->sqrt       = reg_CP_SQRT_PARAM;
    context->sqrt_mode  = (u16) (reg_CP_SQRTCNT & 1);
}