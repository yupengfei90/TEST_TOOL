#ifndef _DAC7565_H
#define _DAC7565_H
#include "sys.h"
#define DAC7565_EN PBout(1)			//DAC7565 串行输入片选信号
#define DAC7565_RSTSEL PEout(10) 	//reset select引脚，置低输入解码为二进郑置高解码为二进制补码
#define DAC7565_SYNC PEout(15)		//sync引脚产生下降沿时，开始接收SPI总线的数据
#endif 
