/*
	power_sw.c
*/
#include "power_sw.h"

/*============================================================================
	���ƣ�power_sw_init
	���ܣ�������KL15,KL30���ϵĹܽ�����Ϊ���ģʽ
  ============================================================================*/
void power_sw_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //ʹ��GPIOD��ʱ��
	
	//KL15_EN,KL30_EN���Ŷ�����Ϊ�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_7|GPIO_Pin_8);	//��ʼ״̬Ϊ0���ر�
}
/*****************************End of power_sw_init*****************************/
