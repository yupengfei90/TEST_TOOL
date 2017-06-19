#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

#define AD7328_CS PBout(0)
#define DAC7565_CS PEout(11)

void SPI2_Init(void);					//初始化SPI2
void SPI2_SetSpeed(u8 SpeedSet);	 	//设置SPI2速度   
u8 SPI2_ReadWriteByte(u8 TxData);		//SPI2读写一个字节	
//void ChipSelect(u8 chip);				//SPI2控制芯片片选
#endif

