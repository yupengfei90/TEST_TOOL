#include "DAC7565.h"
#include "spi.h"

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
}


void DAC7565_Output(u8 channel, float voltage)
{
	DAC7565_EN = 0;
	
}
