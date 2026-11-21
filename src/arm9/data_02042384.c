#include "types.h"

typedef void (*Handler)(void);

extern void func_02024770(void);
extern void func_02024780(void);
extern void func_02024790(void);
extern void func_020247a0(void);
extern void func_020247b0(void);
extern void func_020247c0(void);
extern void func_020247d0(void);
extern void func_020247e0(void);
extern void func_02024890(void);

Handler data_02042384[22] = {
    func_02024890, func_02024890, func_02024890, func_020247a0,
    func_02024790, func_02024780, func_02024770, func_02024890,
    func_020247e0, func_020247d0, func_020247c0, func_020247b0,
    func_02024890, func_02024890, func_02024890, func_02024890,
    func_02024890, func_02024890, func_02024890, func_02024890,
    func_02024890, func_02024890,
};
