#ifndef _LED_H
#define _LED_H
#include "sys.h"


//LED端口定义
#define LED3 PDout(13)
#define LED4 PDout(12)
#define LED5 PDout(14)
#define LED6 PDout(15)


void LED_Init(void);  //初始化
#endif
