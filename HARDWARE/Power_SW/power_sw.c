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
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //使能GPIOE的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //使能GPIOD的时钟
	
	//KL15_EN,KL30_EN,SW_POWER引脚都设置为输出引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //下拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_7|GPIO_Pin_8);	//初始状态为0，关闭
	GPIO_ResetBits(GPIOD,GPIO_Pin_6);	
}
/*****************************End of power_sw_init*****************************/


/*============================================================================
	名称：power_manager
	参数：sel，0:2位分别对应KL15,KL30,SW_POWER的选中与否，1代表选中，0代表未选中
		  state，与sel相对应，1代表ON，2代表OFF
	功能：控制开关KL15,KL30,SW_POWER的ON与OFF
  ============================================================================*/
void power_manager(u8 sel, u8 state)
{
	u8 kl15,kl30,powerSW;
	u8 kl15_st,kl30_st,powerSW_st;
	
	kl15 = sel & 0x01;
	kl30 = sel & 0x02;
	powerSW = sel & 0x04;
	
	kl15_st = state & 0x01;
	kl30_st = state & 0x02;
	powerSW_st = state & 0x04;
	
	if(kl15){
		if(kl15_st)
			KL15_EN = 1;	
		else
			KL15_EN = 0;
	}
	if(kl30){
		if(kl30_st)
			KL30_EN = 1;
		else
			KL30_EN = 0;
	}
	if(powerSW){
		if(powerSW_st)
			SW_POWER = 1;
		else	
			SW_POWER = 0;
	}
}
/*****************************End of power_manager*****************************/
