#ifndef __USART_H__
#define __USART_H__
#include"fix.h"
#include "reg52.h"


void usartInit(void);

void SENT_At(const char *At_Comd);//指针指向At指令
void WIFI_Init(void);


#endif