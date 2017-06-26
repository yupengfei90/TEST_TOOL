#include "HCF4051.h"

void HCF4051_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD的时钟
	
	//HCF4051的通道选择管脚配置为输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void HCF4051_ChannelSelect(u8 channel)
{
	switch(channel)
	{
		/*
			因为外部电路三极管的反向处理，所以原本HCF4051选择通道0，
			HCF4051_A = 0;HCF4051_B = 0;HCF4051_C = 0;
			现变为
			HCF4051_A = 1;HCF4051_B = 1;HCF4051_C = 1;
		*/
		case 0:
			HCF4051_A = 1;HCF4051_B = 1;HCF4051_C = 1;
			break;
		case 1:
			HCF4051_A = 1;HCF4051_B = 1;HCF4051_C = 0;
		break;
		case 2:
			HCF4051_A = 1;HCF4051_B = 0;HCF4051_C = 1;
		break;
		case 3:
			HCF4051_A = 1;HCF4051_B = 0;HCF4051_C = 0;
		break;
		case 4:
			HCF4051_A = 0;HCF4051_B = 1;HCF4051_C = 1;
		break;	
		case 5:
			HCF4051_A = 0;HCF4051_B = 1;HCF4051_C = 0;
		break;
		case 6:
			HCF4051_A = 0;HCF4051_B = 0;HCF4051_C = 1;
		break;	
		case 7:
			HCF4051_A = 0;HCF4051_B = 0;HCF4051_C = 0;
		break;
		default:
			break;	
	}
}

