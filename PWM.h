#ifndef __PWM_H__
#define __PWM_H__
#include "fix.h"
#include "REG52.H"


// 高电平1：使能右电机

#define SPEED_MAX 20

extern unsigned char t ;
extern unsigned int motomode ;
sbit PWM_IN1 = P1 ^ 2; // 高电平1：左电机后退（反转）
sbit PWM_IN2 = P1 ^ 3; // 高电平1：左电机前进（正转）
sbit PWM_IN3 = P1 ^ 6; // 高电平1：右电机前进（正转）
sbit PWM_IN4 = P1 ^ 7; // 高电平1：右电机后退（反转）
sbit PWM_EN1 = P1 ^ 4; // 高电平1：使能左电机
sbit PWM_EN2 = P1 ^ 5;

extern unsigned char
    Speed_L; // 左电机转速调节（调节PWM的一个周期SPEED_MAX*1ms时间内，左电机正转时间：Speed_L*1ms）
extern unsigned char
    Speed_R; // 右电机转速调节（调节PWM的一个周期SPEED_MAX*1ms时间内，右电机正转时间：Speed_R*1ms）
void pwmInit(void);
void moto(void);

#endif
