#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//SPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
 	    													  
void SPI2_Init(void);					//��ʼ��SPI2
void SPI2_SetSpeed(u8 SpeedSet);	 	//����SPI2�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);		//SPI2��дһ���ֽ�	 
#endif
