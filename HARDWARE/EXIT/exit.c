#include "exit.h"
#include "key.h"
#include "delay.h"
#include "led.h"

extern OS_TCB SPI2Task_TCB;

//�жϴ�����
void EXTI0_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS
	OS_ERR err;
	OSIntEnter();
#endif
	delay_ms(10); //�ӳ�10ms,����
	if(KEY_User == 1){
		LED4 = !LED4;
	}
	OSTaskSemPost(&KEYTask_TCB,OS_OPT_POST_NONE,&err);
	OSTaskSemPost(&SPI2Task_TCB,OS_OPT_POST_NONE,&err);
	EXTI_ClearITPendingBit(EXTI_Line0);
	
#if SYSTEM_SUPPORT_OS
	OSIntExit();
#endif
}

void exit0_init()
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);//ʹ��SYSCFGģ����ʹ���ⲿ�ж����üĴ���
	
	KEY_Init();
	
	//ѡ��GPIOA_Pin0ӳ�䵽�ж���0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	
		//�����ⲿ�жϹܽţ�ʹ��, ģʽ
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	//��NVIC�ж�Ӧ���ⲿ�ж��߼������ж����ȼ�
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
}
