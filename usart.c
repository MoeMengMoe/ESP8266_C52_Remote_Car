#include"fix.h"
#include"usart.h"
#include"reg52.h"
#include "delay.h"
#include "hardware.h"


void usartInit(){
    SCON = 0x50;//串行口中断方式一，且启动串行口接受（REN=1）
	TMOD = 0x20;//计数器1工作方式2，自动重装载
	
	TH1 = 0xfd;//设置波特率与esp8266达成一致
	TL1 = TH1;
	
	PCON = 0;//波特率不加倍
	
	TR1 = 1;//启动计数器
	EA = 1;//开总中断

}


void SENT_At(const char *At_Comd)//指针指向At指令
{
	ES = 0;//关闭串口中断
	while(*At_Comd!='\0')
	{
		SBUF = *At_Comd;
		while(!TI);//等待该字节发送完毕，发送完后硬件自动置一TI
		TI = 0;//硬件之以后必须软件置零才能进行下一次数据传送
		delay_us(5);
		At_Comd++;//指向下一个字节
	}
}

void WIFI_Init()//通过单片机配置AT指令
{
	SENT_At("AT\r\n");//测试
//	led1=0;//发送成功后亮灯
	delay_ms(500);
	SENT_At("AT+CWMODE=1\r\n");//设置模块 WIFI 模式为 STA 模式
	led1=0;//发送成功后亮灯
//	led2=0;//发送成功后亮灯
	delay_ms(500);
	SENT_At("AT+RST\r\n");//重启模块生效
//	led1=0;//发送成功后亮灯
//	led2=0;//发送成功后亮灯
	delay_ms(2000);
	SENT_At("AT+CWJAP=\"SHYQ-4#-603\",\" \"\r\n");//连接 WIFI热点，路由名字：RSP8266，密码：12345123
 	led1=1;//发送成功后灭灯
	led2=0;//发送成功后亮灯
//	led3=0;//发送成功后亮灯
	delay_ms(2000);
	delay_ms(2000);
	SENT_At("AT+CIPSTART=\"TCP\",\"172.25.67.245\",8080\r\n");//建立 TCP 连接到” 192.168.xx.xxx”,8088
//	led1=0;//发送成功后亮灯
//	led2=1;//发送成功后灭灯
//	led3=0;//发送成功后亮灯
	delay_ms(2000);
	SENT_At("AT+CIPMODE=1\r\n");//开启透传模式
	led1=0;//发送成功后亮灯
	led2=0;//发送成功后亮灯
    SENT_At("AT+CIPSEND\r\n");//开启透传模式
//	led3=0;//发送成功后亮灯
}

