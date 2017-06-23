#ifndef _DAC7565_H
#define _DAC7565_H
#include "sys.h"

#define DAC7565_EN PBout(1)			//DAC7565 串行输入片选信号
#define DAC7565_RSTSEL PEout(10) 	//reset select引脚，置低输入解码为二进郑置高解码为二进制补码
#define DAC7565_SYNC PEout(15)		//sync引脚产生下降沿时，开始接收SPI总线的数据
typedef union{
	u32 data;
	struct{
		u32 db0:1;	//db0-db3 don't care bits
		u32 db1:1;
		u32 db2:1;
		u32 db3:1;
		u32 d0:1;	//d0-d11为数据位
		u32 d1:1;
		u32 d2:1;
		u32 d3:1;
		u32 d4:1;
		u32 d5:1;
		u32 d6:1;
		u32 d7:1;
		u32 d8:1;
		u32 d9:1;
		u32 d10:1;
		u32 d11:1;
		u32 pd0:1;	//节能模式选择，1为节能，0为正常
		u32 dac_select0:1;	//输出通道选择位0
		u32 dac_select1:1;	//输出通道选择位1
		u32 db19:1;	//固定为0
		u32 ld0:1;	//控制模拟输出
		u32 ld1:1;	//控制模拟输出
		u32 db22:1;	//固定为0
		u32 db23:1; //固定为0
	}bits;
}DAC7565_Seq_t;	//DAC7565接收的24位串行数据的格式

/* -------------------------------------------
	--Global Function
   ------------------------------------------- */
void DAC7565_Init(void);
void DAC7565_Output(u8 channel, double voltage);

#endif 
