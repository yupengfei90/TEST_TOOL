#ifndef _LED_H
#define _LED_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//LED�˿ڶ���
//#define LED0 PFout(9)
//#define LED1 PFout(10)

#define LED3 PDout(13)
#define LED4 PDout(12)
#define LED5 PDout(14)
#define LED6 PDout(15)


void LED_Init(void);  //��ʼ��
#endif
