#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include"fix.h"
#include "reg52.h"
// #include <intrins.h>


// --- VS Code 语法兼容处理 ---


// 按照你的历程习惯定义：SS=段选(P2.6), CS=位选(P2.7)
sbit SS = P2^6;	   
sbit CS = P2^7;	   

// 全局显示缓冲区 (ZY-1 为 6 位数码管)
extern unsigned char DispBuf[6];
extern bit timer_start_flag;

// 函数声明
void DispInit(void);
void DispClear(void);

#endif