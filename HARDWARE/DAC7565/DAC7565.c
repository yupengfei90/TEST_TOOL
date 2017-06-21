#include "DAC7565.h"
#include "spi.h"

void DAC7565_Init(void)
{
	//DAC7565�������ų�ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOB��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //ʹ��GPIOE��ʱ��
	
	//74HCT595��RSTSEL,SYNC(-),EN(-)���Ŷ�����Ϊ�������,(-)��ʾ����Ч
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	DAC7565_RSTSEL = 0;		//�������Ϊbinary-straight	
	DAC7565_EN = 1;
	DAC7565_SYNC = 1;
}


void DAC7565_Output(u8 channel, float voltage)
{
	DAC7565_EN = 0;
	
}
