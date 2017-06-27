/*
	74hc595.c
*/
#include "74hc595.h"
#include "delay.h"

/* ----------------------------------------------------------------------------
   -- 名称：Init_74HC595 
   -- 功能： 将74HCT595的DS,SHCP,STCP,MR引脚都设置为输出引脚
   ---------------------------------------------------------------------------- */
void Init_74HC595(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //使能GPIOD的时钟
	
	//74HCT595的DS,SHCP,STCP,MR引脚都设置为输出引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //下拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOE, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15);	//DS,SHCP,STCP初始置0
	MR = 0; 	//shift register清零
	delay_us(1);
	MR = 1;	
}
/*<!****** End of Init_74HC595() **********/


/* ----------------------------------------------------------------------------
   -- 名称：SW_Open 
   -- 功能：打开指定开关，一位代表一个开关，详见74hc595.h的宏定义，
	可以用 | 将多个开关同时打开
   ---------------------------------------------------------------------------- */
void SW_Open(u32 state)
{
	u8 i;
	SHCP = 0;
	delay_us(1);
	for(i=0;i<32;i++){
		if(((state >> i) & 0x01))	//低位先出
			DS = 1;
		else
			DS = 0;
		delay_us(1);
		SHCP = 1;
		delay_us(1);
		SHCP = 0;
		delay_us(1);
	}
	
	//开始并行输出
	STCP = 0;
	delay_us(1);
	STCP = 1;
	delay_us(1);
	STCP = 0;
}
/*<!****** End of SW_Open() **********/
