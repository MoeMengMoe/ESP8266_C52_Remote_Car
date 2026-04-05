#include"fix.h"
#include"PWM.h"
#include"REG52.H"

unsigned char t=0;
unsigned int i=0;
unsigned int motomode=0;


unsigned char Speed_L;		//左电机转速调节（调节PWM的一个周期SPEED_MAX*1ms时间内，左电机正转时间：Speed_L*1ms）
unsigned char Speed_R;	    //右电机转速调节（调节PWM的一个周期SPEED_MAX*1ms时间内，右电机正转时间：Speed_R*1ms）
void moto()
{

switch(motomode)
{
	case 0:
		t=0;
		PWM_IN1 = 0;
		PWM_IN2 = 0; 		
		PWM_IN3 = 0;                               
		PWM_IN4 = 0;
		break;
	case 1:
		t++;
	  if ( t >= 20 ) 
		   t = 0;				//PWM波的周期为：SPEED_MAX*1ms = 20ms

		if ( t < Speed_L )		//PWM波高电平时间：(Speed_L)*1ms
		{
			PWM_IN1 = 0;
			PWM_IN2 = 1; 		//左电机的正转
		}
		else					//PWM波低电平时间：（SPEED_MAX-Speed_L）*1ms
		{
    PWM_IN1 = 0;
		PWM_IN2 = 0; 		//左电机的停转
		}
	
		if ( t < Speed_R )		//PWM波高电平时间：Speed_R*1ms
		{
		PWM_IN3 = 1;
		PWM_IN4 = 0;		//右电机的正转		
		}
		else					//PWM波低电平时间：（SPEED_MAX-Speed_R）*1ms
		{
		PWM_IN3 = 0;
		PWM_IN4 = 0;		//右电机的停转	
		}
		break;
	case 2:
		t++;
	  if ( t >= 20 ) 
		   t = 0;				//PWM波的周期为：SPEED_MAX*1ms = 20ms

		if ( t < Speed_L )		//PWM波高电平时间：(Speed_L)*1ms
		{
			PWM_IN1 = 1;
			PWM_IN2 = 0; 		//左电机的反转
		}
		else					//PWM波低电平时间：（SPEED_MAX-Speed_L）*1ms
		{
    PWM_IN1 = 0;
		PWM_IN2 = 0; 		//左电机的停转
		}
	
		if ( t < Speed_R )		//PWM波高电平时间：Speed_R*1ms
		{
		PWM_IN3 = 0;
		PWM_IN4 = 1;		//右电机的反转		
		}
		else					//PWM波低电平时间：（SPEED_MAX-Speed_R）*1ms
		{
		PWM_IN3 = 0;
		PWM_IN4 = 0;		//右电机的停转	
		}
		break;
	case 3:
		t++;
	  if ( t >= 20 ) 
		   t = 0;				//PWM波的周期为：SPEED_MAX*1ms = 20ms

		if ( t < Speed_L )		//PWM波高电平时间：(Speed_L)*1ms
		{
			PWM_IN1 = 0;
			PWM_IN2 = 1; 		//左电机的反转
		}
		else					//PWM波低电平时间：（SPEED_MAX-Speed_L）*1ms
		{
    PWM_IN1 = 0;
		PWM_IN2 = 0; 		//左电机的停转
		}
	
		PWM_IN3 = 0;
		PWM_IN4 = 0;		//右电机的反转		
		
		break;
	case 4:
		t++;
	  if ( t >= 20 ) 
		   t = 0;				//PWM波的周期为：SPEED_MAX*1ms = 20ms
			PWM_IN1 = 0;
			PWM_IN2 = 0; 		//左电机的反转	
		if ( t < Speed_R )		//PWM波高电平时间：Speed_R*1ms
		{
		PWM_IN3 = 1;
		PWM_IN4 = 0;		//右电机的反转		
		}
		else					//PWM波低电平时间：（SPEED_MAX-Speed_R）*1ms
		{
		PWM_IN3 = 0;
		PWM_IN4 = 0;		//右电机的停转	
		}
		break;
	default:
		t=0;
		PWM_IN1 = 0;
		PWM_IN2 = 0; 		//左电机的停转
		PWM_IN3 = 0;                               
		PWM_IN4 = 0;
}
}

void pwmInit(){
	PWM_EN1 = 1;
	PWM_EN2 = 1;		//电机无效
	PWM_IN1 = 0;
	PWM_IN2 = 0; 		//左电机的停转
	PWM_IN3 = 0;                               
	PWM_IN4 = 0;		//右电机的停转
	Speed_L = 10;		//设置左电机PWM初值（初始转速）
	Speed_R = 10;		//设置右电机PWM初值

}