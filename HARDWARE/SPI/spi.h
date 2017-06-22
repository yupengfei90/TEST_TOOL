#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

void SPI2_Init(void);					//��ʼ��SPI2
void SPI2_SetSpeed(u8 SpeedSet);	 	//����SPI2�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);		//SPI2��дһ���ֽ�	
//void ChipSelect(u8 chip);				//SPI2����оƬƬѡ


void SPI_Config(void);
void MySPI_SendData(char da);
#endif

