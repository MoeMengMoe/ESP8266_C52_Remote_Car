#include"fix.h"
#include "delay.h"
typedef unsigned char uchar;
typedef unsigned int uint;



void delay_ms(uint ms)
{
	volatile uint i, j;
	for(i=ms;i>0;i--)
	for(j=460;j>0;j--);   // 11.0592MHz 晶振，每轮约1ms
}

void delay_us(uchar us)
{
	volatile uchar k;
	while(us--)
	for(k=5;k>0;k--);     // 每轮约1us
}
