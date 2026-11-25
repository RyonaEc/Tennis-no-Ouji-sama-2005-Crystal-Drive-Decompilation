#include "types.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    u16 flag;
    u16 pad;
    u16 *buffer;
} PMiRegisterBuffer;

typedef struct {
    u8 pad[0xc];
    u32 *work;
} PMiWork;

extern PMiRegisterBuffer data_0204acdc[4];
extern u32 data_0204aca4;
extern u32 data_0204aca8;
extern PMiWork data_0204acb8;
extern void func_0202c36c(u32 result);

void func_0202c1bc(u32 tag, u32 data, u32 err)
{
    u16 command;
    u16 pxiResult;

    if (err) {
        func_0202c36c(2);
        return;
    }

    command = (u16)((data & 0x00007f00) >> 8);
    pxiResult = (u16)((data & 0x000000ff) >> 0);

    if (0x70 <= command && command <= 0x73) {
        int num = (int)(command - 0x70);
        u16 value = (u16)(pxiResult & 0xff);
        u16 *buffer = data_0204acdc[num].buffer;

        if (buffer)
            *buffer = value;

        data_0204acdc[num].flag = 1;
        pxiResult = 0;
    } else if (command == 0x60) {
        data_0204aca4 = 1;
    } else if (command == 0x62) {
        data_0204aca8 = 1;
    } else if (command == 0x67) {
        if (data_0204acb8.work)
            *data_0204acb8.work = pxiResult;
        pxiResult = 0;
    }

    func_0202c36c(pxiResult);
}