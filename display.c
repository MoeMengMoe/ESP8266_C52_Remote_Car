#include"fix.h"
#include "display.h"
#include <intrins.h>
#include "hardware.h"


// 对应参考程序中的字型数据：0-9, A-F, -(16), P(17)
code unsigned char Tab[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
    0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x40, 0x73
};

unsigned char DispBuf[6];
unsigned char sec = 0;
bit timer_start_flag = 0;

void DispClear() {
    unsigned char i;
    for (i = 0; i < 6; i++) {
        DispBuf[i] = 0x00; // 0x00 为全灭
    }
}

void DispInit() {
    DispClear();
    TMOD = (TMOD & 0xF0) | 0x01; // Timer0 16位模式，用于显示扫描和电机PWM
    EA = 0;
    TH0 = 0xFC;
    TL0 = 0x66;
    ET0 = 1;
    EA = 1;
    TR0 = 1;
    // PS = 1;  // 串口中断高优先级会导致Timer0 PWM中断被阻塞


	led1 = 1;
	led2 = 1;
}
