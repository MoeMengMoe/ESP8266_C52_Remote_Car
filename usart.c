#include"fix.h"
#include"usart.h"
#include"reg52.h"
#include "delay.h"
#include "hardware.h"


void usartInit(){
    SCON = 0x50;//串行口中断方式一，且启动串行口接受（REN=1）
	TMOD = (TMOD & 0x0F) | 0x20;//保留T0配置，计数器1工作方式2，自动重装载

	TH1 = 0xfd;//设置波特率与esp8266达成一致
	TL1 = TH1;

	PCON = 0;//波特率不加倍

	TR1 = 1;//启动计数器
	EA = 1;//开总中断
	ES = 1;//开串口中断
}


void SENT_At(const char *At_Comd)//指针指向At指令
{
	bit es_saved = ES;//保存当前ES状态
	ES = 0;//关闭串口中断
	while(*At_Comd!='\0')
	{
		SBUF = *At_Comd;
		while(!TI);//等待该字节发送完毕，发送完后硬件自动置一TI
		TI = 0;//硬件之以后必须软件置零才能进行下一次数据传送
		delay_us(5);
		At_Comd++;//指向下一个字节
	}
	ES = es_saved;//恢复ES状态
}

void WIFI_Init()
{
	SENT_At("AT\r\n");
	delay_ms(1000);

	SENT_At("AT+CWMODE=1\r\n");
	led1 = 0;
	delay_ms(1000);

	SENT_At("AT+CIPSTA=\"192.168.31.138\",\"192.168.31.1\",\"255.255.255.0\"\r\n");
	delay_ms(1000);

	SENT_At("AT+CWJAP=\"Xiaomi_CC47\",\"405405405\"\r\n");
	led1 = 1;
	led2 = 0;
	delay_ms(2000);
	// delay_ms(2000);

	SENT_At("AT+CIPSTART=\"TCP\",\"192.168.31.221\",8080\r\n");
	delay_ms(2000);

	SENT_At("AT+CIPMODE=1\r\n");
	led1 = 0;
	led2 = 0;
	delay_ms(1000);
	SENT_At("AT+CIPSEND\r\n");
}
