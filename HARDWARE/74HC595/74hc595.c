/*
	74hc595.c
*/
#include "74hc595.h"
#include "delay.h"

/* ----------------------------------------------------------------------------
   -- ���ƣ�Init_74HC595 
   -- ���ܣ� ��74HCT595��DS,SHCP,STCP,MR���Ŷ�����Ϊ�������
   ---------------------------------------------------------------------------- */
void Init_74HC595(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //ʹ��GPIOD��ʱ��
	
	//74HCT595��DS,SHCP,STCP,MR���Ŷ�����Ϊ�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOE, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15);	//DS,SHCP,STCP��ʼ��0
	MR = 0; 	//shift register����
	delay_us(1);
	MR = 1;	
}
/*<!****** End of Init_74HC595() **********/


/* ----------------------------------------------------------------------------
   -- ���ƣ�SW_Open 
   -- ���ܣ���ָ�����أ�һλ����һ�����أ����74hc595.h�ĺ궨�壬
	������ | ���������ͬʱ��
   ---------------------------------------------------------------------------- */
void SW_Open(u32 state)
{
	u8 i;
	SHCP = 0;
	delay_us(1);
	for(i=0;i<32;i++){
		if(((state >> i) & 0x01))	//��λ�ȳ�
			DS = 1;
		else
			DS = 0;
		delay_us(1);
		SHCP = 1;
		delay_us(1);
		SHCP = 0;
		delay_us(1);
	}
	
	//��ʼ�������
	STCP = 0;
	delay_us(1);
	STCP = 1;
	delay_us(1);
	STCP = 0;
}
/*<!****** End of SW_Open() **********/
