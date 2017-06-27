#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

/*===============================================================
	Global Function
  ===============================================================*/
void SPI2_Init(void);					//��ʼ��SPI2
void SPI2_SetSpeed(u8 SpeedSet);	 	//����SPI2�ٶ�   
void MySPI_SendData(char da);
u8 MySPI_ReceiveData(void);
u8 SPI2_ReadWriteByte(u8 TxData);		//SPI2��дһ���ֽ�,����

#endif

