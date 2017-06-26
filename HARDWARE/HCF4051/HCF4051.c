#include "HCF4051.h"

void HCF4051_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOD��ʱ��
	
	//HCF4051��ͨ��ѡ��ܽ�����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void HCF4051_ChannelSelect(u8 channel)
{
	switch(channel)
	{
		/*
			��Ϊ�ⲿ��·�����ܵķ�����������ԭ��HCF4051ѡ��ͨ��0��
			HCF4051_A = 0;HCF4051_B = 0;HCF4051_C = 0;
			�ֱ�Ϊ
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
