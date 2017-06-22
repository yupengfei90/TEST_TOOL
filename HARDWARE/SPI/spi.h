#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

void SPI2_Init(void);					//初始化SPI2
void SPI2_SetSpeed(u8 SpeedSet);	 	//设置SPI2速度   
u8 SPI2_ReadWriteByte(u8 TxData);		//SPI2读写一个字节	
//void ChipSelect(u8 chip);				//SPI2控制芯片片选


void SPI_Config(void);
void MySPI_SendData(char da);
#endif

