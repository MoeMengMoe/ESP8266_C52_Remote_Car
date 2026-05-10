#include "PWM.h"
#include"fix.h"
#include "display.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include"hardware.h"


typedef unsigned char uchar;
typedef unsigned int uint;




// 声明外部资源
extern code unsigned char Tab[];
extern unsigned char DispBuf[6];
extern bit timer_start_flag;


uchar Recive_table[15];
uint i=0;




void T0INTSVC() interrupt (1)	//定时器1的中断号为：1
{
	code unsigned char com[] = {0x20,0x10,0x08,0x04,0x02,0x01};	//显示位的端口控制字节
	static unsigned char n = 0;						//n: 扫描显示位计数,0-7

//================数码管定时扫描驱动显示===============	
	TR0 = 0;
	TH0 = 0xFC;
	TL0 = 0x66;						//可以将FC66换成0000，降低扫描速度，观察和理解动态扫描
	TR0 = 1;
	P0 = 0xFF;		//消隐
	CS = 1;
	CS = 0;
	P0 = DispBuf[n];	//更新扫描显示数据
	SS = 1;
	SS = 0;
	P0 = ~com[n];	//重新显示
	CS = 1;
	CS = 0;
	n++;					 	//指向下一位扫描显示
	if(n>=6) n= 0x00;
//====================================================
	moto();
}

void Uart() interrupt (4) 
{
    if(RI)
    {
        RI = 0;
        Recive_table[i] = SBUF; 

        // 1. 找寻帧头：只有第一个字节是 '-' 才开始计数
        if(Recive_table[0] == '-')
        {
            i++;
            // 收到帧头，先灭灯提示
            led1 = 0; 
            led2 = 0; 
        }
        else
        {
            i = 0; // 帧头不对，重置
        }

        // 2. 接收满 9 个字节（一帧完整指令）
        if(i >= 9)
        {
            // 3. 校验特征码：是否为 "-1Pd"
            if(Recive_table[1]=='1' && Recive_table[2]=='P' && Recive_table[3]=='d')
            {
                // --- 原注释功能：数码管反馈 ---
                DispBuf[5] = Tab[16]; // 显示 '-'
                DispBuf[4] = Tab[1];
                DispBuf[3] = Tab[17];
                DispBuf[2] = Tab[13];

                // --- 原注释功能：动作模式切换 ---
                // 注意：串口收到的是字符 '0'，必须加单引号匹配
                switch(Recive_table[7]) 
                {
                    case '0': motomode = 0; led2 = 1; break; // 停止
                    case '1': motomode = 1; led2 = 0; break; // 前进
                    case '2': motomode = 2; led2 = 0; break; // 后退
                    case '3': motomode = 3; led2 = 0; break; // 左转
                    case '4': motomode = 4; led2 = 0; break; // 右转
                    default:  led1 = 1; break; // 异常报错
                }

                // --- 原注释功能：精细开关灯逻辑 ---
                if(Recive_table[4] == '0') 
                {
                    if(Recive_table[5] == '1') { led2 = 0; led1 = 1; DispBuf[0] = Tab[1]; }
                    if(Recive_table[5] == '0') { led2 = 1; led1 = 0; DispBuf[0] = Tab[0]; }
                }
            }
            
            i = 0; // 接收完成，重置索引，等待下一包
        }
    }
    else
    {
        TI = 0;
    }
}

int main() {
    uint heartbeat_ticks = 0;

    DispInit(); // 内部已包含定时器初始化
    pwmInit();  // 内部已包含电机初始化
    KeyInit();
    delay_ms(100); // 等待系统稳定
    // 开机自检：显示 123456 
    // ... (你的自检逻辑) ...
    DispBuf[5] = Tab[1];
	DispBuf[4] = Tab[2];
	DispBuf[3] = Tab[3];
	DispBuf[2] = Tab[4];
	DispBuf[1] = Tab[5];
	DispBuf[0] = Tab[6];
    delay_ms(2000);		
    usartInit();
	WIFI_Init();
    
    timer_start_flag = 1;
    delay_ms(3000);  // 等待 TCP 透传链路稳定

    while(1) {
        KeyScanMotor();

        if(heartbeat_ticks >= 200) {
            heartbeat_ticks = 0;
            led1 = 1;
            led2 = 1;
            SENT_At("12345\n");//发5字节“12345”给手机
            ES = 1;
            buzzer = 0;
            delay_ms(20);
            buzzer = 1;
        }

        delay_ms(10);
        heartbeat_ticks++;
    }
}
