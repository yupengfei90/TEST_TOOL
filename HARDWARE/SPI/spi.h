#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

/*===============================================================
	Global Function
  ===============================================================*/
void SPI2_Init(void);					//初始化SPI2
void SPI2_SetSpeed(u8 SpeedSet);	 	//设置SPI2速度   
void MySPI_SendData(char da);
u8 MySPI_ReceiveData(void);
u8 SPI2_ReadWriteByte(u8 TxData);		//SPI2读写一个字节,慎用

#endif

