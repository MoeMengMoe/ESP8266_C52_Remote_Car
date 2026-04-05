#include"fix.h"
#include "delay.h"
typedef unsigned char uchar;
typedef unsigned int uint;



void delay_ms(uint ms)
{
	uchar i,j;
	for(i=ms;i>0;i--)
	for(j=120;j>0;j--);
}
 
void delay_us(uchar us)
{
	while(us--);
}