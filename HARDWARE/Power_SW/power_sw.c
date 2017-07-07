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
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //ʹ��GPIOE��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //ʹ��GPIOD��ʱ��
	
	//KL15_EN,KL30_EN,SW_POWER���Ŷ�����Ϊ�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_7|GPIO_Pin_8);	//��ʼ״̬Ϊ0���ر�
	GPIO_ResetBits(GPIOD,GPIO_Pin_6);	
}
/*****************************End of power_sw_init*****************************/


/*============================================================================
	���ƣ�power_manager
	������sel��0:2λ�ֱ��ӦKL15,KL30,SW_POWER��ѡ�����1����ѡ�У�0����δѡ��
		  state����sel���Ӧ��1����ON��2����OFF
	���ܣ����ƿ���KL15,KL30,SW_POWER��ON��OFF
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
