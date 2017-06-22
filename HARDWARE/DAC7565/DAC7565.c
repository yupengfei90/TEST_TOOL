#include "DAC7565.h"
#include "spi.h"
#include "delay.h"

DAC7565_Seq_t DAC7565_Seq = {0};

//DAC7565_Seq.bits.ld0 = 1;
//DAC7565_Seq.bits.ld1 = 0;

void DAC7565_Init(void)
{
	//DAC7565控制引脚初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //使能GPIOE的时钟
	
	//74HCT595的RSTSEL,SYNC(-),EN(-)引脚都设置为输出引脚,(-)表示低有效
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	DAC7565_RSTSEL = 0;		//输入解码为binary-straight	
	DAC7565_EN = 1;
	DAC7565_SYNC = 1;
	
	//设定串行发送的固定格式
	DAC7565_Seq.bits.pd0 = 0;	//非节能模式 
	DAC7565_Seq.bits.ld0 = 0; 	//设置为Single-channel update
	DAC7565_Seq.bits.ld1 = 1;
	
	delay_us(1);
}


void DAC7565_Output(u8 channel, double voltage)
{
	u16 ad;
	//设置输出通道
	DAC7565_Seq.data &= (~(0x3<<17));
	DAC7565_Seq.data |= ((channel & 0x3)<<17);
	//设置输出AD值
	ad = (u16)((voltage/2.5)*4095);
	DAC7565_Seq.data &= (~(0x0FFF<<4));
	DAC7565_Seq.data |= (ad<<4);
	
	DAC7565_EN = 0;
	delay_us(1);
	DAC7565_SYNC = 0;
	delay_us(1);
	SPI2_ReadWriteByte((u8)(DAC7565_Seq.data >> 16));
	SPI2_ReadWriteByte((u8)((DAC7565_Seq.data >> 8)&0xFF));
	SPI2_ReadWriteByte((u8)(DAC7565_Seq.data&0xFF));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){;}//等待发送区空 	
	delay_us(40);	//故意加长的延时，因为SPI2波特率较低时会出现意外的SYNC上升沿早于数据传输完毕
	DAC7565_SYNC = 1;
	delay_us(1);
	DAC7565_EN = 1;
	delay_us(1);
}
