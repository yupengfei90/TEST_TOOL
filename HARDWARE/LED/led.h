#ifndef _LED_H
#define _LED_H
#include "sys.h"


//LED�˿ڶ���
#define LED3 PDout(13)
#define LED4 PDout(12)
#define LED5 PDout(14)
#define LED6 PDout(15)


void LED_Init(void);  //��ʼ��
#endif
