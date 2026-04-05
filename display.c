#include"fix.h"
#include "display.h"
#include <intrins.h>
#include "hardware.h"


// 对应你历程中的字型数据：0-9, A-F, H(16), L(17), P(18), -(19)
code unsigned char Tab[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
    0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x76, 0x38, 0x73, 0x40
};

unsigned char DispBuf[6];
unsigned char sec = 0;
bit timer_start_flag = 0;

/* 定时器1中断服务函数：处理扫描与计时 */
void T1INTSVC() interrupt (3) 
{
    // ZY-1 位选控制字节，对应 WE1-WE6 
    code unsigned char com[] = {0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
    static unsigned char n = 0;
    static unsigned int counter_1ms = 0;

    // 1. 重装载 1ms 初值
    TR1 = 0;
    TH1 = 0xFC;
    TL1 = 0x66;
    TR1 = 1;

    // 2. 消隐 (关闭位选) 
    P0 = 0xFF;
    CS = 1; _nop_(); CS = 0;

    // 3. 更新段码 (内容)
    P0 = DispBuf[n];
    SS = 1; _nop_(); SS = 0;

    // 4. 重新显示 (位选送低电平) 
    P0 = ~com[n];
    CS = 1; _nop_(); CS = 0;

    n++;
    if(n >= 6) n = 0;

    // 5. 联动跑表逻辑
    if(timer_start_flag) {
        counter_1ms++;
        if(counter_1ms >= 100) {
            counter_1ms = 0;
            sec++;
            if(sec >= 120) sec = 0;
            // 可以在此处更新特定的 DispBuf 位置
        }
    }
}

void DispClear() {
    unsigned char i;
    for (i = 0; i < 6; i++) {
        DispBuf[i] = 0x00; // 0x00 为全灭
    }
}

void DispInit() {
    DispClear();
    TMOD = 0x11; // 定时器0和1均为16位模式
    TH1 = 0xFC;
    TL1 = 0x66;
    ET1 = 1;     // 使能T1中断
    EA = 1;      // 使能总中断
    TR1 = 1;     // 启动T1


	led1 = 1;
	led2 = 1;
}