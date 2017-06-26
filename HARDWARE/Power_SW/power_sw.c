/*
	power_sw.c
*/
#include "power_sw.h"

/*============================================================================
	名称：power_sw_init
	功能：将控制KL15,KL30开断的管脚配置为输出模式
  ============================================================================*/
void power_sw_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //使能GPIOD的时钟
	
	//KL15_EN,KL30_EN引脚都设置为输出引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //下拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_7|GPIO_Pin_8);	//初始状态为0，关闭
}
/*****************************End of power_sw_init*****************************/
