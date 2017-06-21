#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "exit.h"
#include "sram.h"
#include "can.h"
#include "spi.h"
#include "AD7328.h"
#include "74hc595.h"
#include "malloc.h"
#include "includes.h"


//Э�鲿��
u8 cmd[32] = {0};
u8 cmd_head[] = {0xFF, 0xAA};
u8 cmd_tail[] = {0xFF, 0x55};

//Start task Prio
#define START_TASK_PRIO 3
#define START_STK_SIZE 128
OS_TCB StartTask_TCB;
CPU_STK StartTask_STK[START_STK_SIZE];
void start_task(void *p_arg);

//LED3 task
#define LED3_TASK_PRIO 4
#define LED3_STK_SIZE 128
OS_TCB LED3Task_TCB;
CPU_STK LED3Task_STK[LED3_STK_SIZE];
void led3_task(void *p_arg);

//USART2REC task
#define USART2REC_TASK_PRIO 5
#define USART2REC_STK_SIZE 128
OS_TCB 	USART2RECTask_TCB;
CPU_STK USART2RECTask_STK[USART2REC_STK_SIZE];
void usart2rec_task(void *p_arg);

//key down response
#define KEY_TASK_PRIO 6
#define KEY_STK_SIZE 128
OS_TCB KEYTask_TCB;
CPU_STK KEYTask_STK[LED3_STK_SIZE];
void key_task(void *p_arg);

//SPI2 test task
#define SPI2_TASK_PRIO 7
#define SPI2_STK_SIZE 128
OS_TCB SPI2Task_TCB;
CPU_STK SPI2Task_STK[SPI2_STK_SIZE];
void spi2_task(void *p_arg);

//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);  	//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	uart_init(115200);  //���ڳ�ʼ��
//	uart_init(2100000);
	LED_Init();         //LED��ʼ��	
	exit0_init();
//	LCD_Init();			//LCD��ʼ��	
//	FSMC_SRAM_Init();	//��ʼ��SRAM
	my_mem_init(SRAMIN);//��ʼ���ڲ�RAM
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);//CAN��ʼ������ģʽ,������500Kbps 	
	SPI2_Init();
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4); //����ģʽ(42/4)M SPI
	AD7328_Init();
	Init_74HC595();

	SW_Open(0x55555555);
	while(1);
	OSInit(&err);		    	//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���			 
	//������ʼ����
	OSTaskCreate (  (OS_TCB        *)&StartTask_TCB,
					(CPU_CHAR      *)"start task",
					(OS_TASK_PTR    )start_task,
					(void          *)0,
					(OS_PRIO        )START_TASK_PRIO,
					(CPU_STK       *)StartTask_STK,
					(CPU_STK_SIZE   )START_STK_SIZE/10,
					(CPU_STK_SIZE   )START_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
	
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
}


void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	
	OS_CRITICAL_ENTER();
	//����LED3����
	OSTaskCreate (  (OS_TCB        *)&LED3Task_TCB,
					(CPU_CHAR      *)"led3 task",
					(OS_TASK_PTR    )led3_task,
					(void          *)0,
					(OS_PRIO        )LED3_TASK_PRIO,
					(CPU_STK       *)LED3Task_STK,
					(CPU_STK_SIZE   )LED3_STK_SIZE/10,
					(CPU_STK_SIZE   )LED3_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
					
	
	//USART2���մ�������
	OSTaskCreate (  (OS_TCB        *)&USART2RECTask_TCB,
					(CPU_CHAR      *)"usart2rec task",
					(OS_TASK_PTR    )usart2rec_task,
					(void          *)0,
					(OS_PRIO        )USART2REC_TASK_PRIO,
					(CPU_STK       *)USART2RECTask_STK,
					(CPU_STK_SIZE   )USART2REC_STK_SIZE/10,
					(CPU_STK_SIZE   )USART2REC_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
					
	//��������������Ӧ����
	OSTaskCreate (  (OS_TCB        *)&KEYTask_TCB,
					(CPU_CHAR      *)"key task",
					(OS_TASK_PTR    )key_task,
					(void          *)0,
					(OS_PRIO        )KEY_TASK_PRIO,
					(CPU_STK       *)KEYTask_STK,
					(CPU_STK_SIZE   )KEY_STK_SIZE/10,
					(CPU_STK_SIZE   )KEY_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
					
	//����SPI2��������
	OSTaskCreate (  (OS_TCB        *)&SPI2Task_TCB,
					(CPU_CHAR      *)"spi2 task",
					(OS_TASK_PTR    )spi2_task,
					(void          *)0,
					(OS_PRIO        )SPI2_TASK_PRIO,
					(CPU_STK       *)SPI2Task_STK,
					(CPU_STK_SIZE   )SPI2_STK_SIZE/10,
					(CPU_STK_SIZE   )SPI2_STK_SIZE,
					(OS_MSG_QTY     )0,
					(OS_TICK        )0,
					(void          *)0,
					(OS_OPT         )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					(OS_ERR        *)&err);
					
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSTaskDel((OS_TCB *)0, &err);
}

//LED3��˸����
void led3_task(void *p_arg)
{
	OS_ERR err;
//	u32 i;
	
	while(1)
	{
		LED3 = ~LED3;
		
		SW_Open(0x55555555);
//		for(i=0;i<0xFFFFFF;i++)
//			;
//		printf("CpuUsage %u\n",OSStatTaskCPUUsage);
//		printf("KEYTask_TCB CpuUsage %u\n",KEYTask_TCB.CPUUsage);
//		printf("1234567\n");
		
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


//USART2���մ�������
void usart2rec_task(void *p_arg)
{
	OS_ERR err;
	u16 t, len;
	
	while(1){
		if(USART_RX_STA & 0x8000){
			//printf("\r\n���յ�������Ϊ: \r\n");
			len = (USART_RX_STA & 0x3FF);
			for(t=0;t<len;t++){
				printf("%X ",USART_RX_BUF[t]);
			}
			printf("\r\n");
			
			if (USART_RX_BUF[0] == cmd_head[0] && USART_RX_BUF[1] == cmd_head[1] &&  \
				USART_RX_BUF[len-2] == cmd_tail[0] && USART_RX_BUF[len-1] == cmd_tail[1])
			{
				printf("��ȷ�������ʽ\r\n");
			}
			USART_RX_STA = 0;
		}		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


//�û�����������Ӧ����������
void key_task(void *p_arg)
{
	OS_ERR err;
	u8 i=0,res=0;
	u8 cnt=0;
	u8 canbuf[8];
	
	while(1){
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
		printf("\r\n���͵�����Ϊ:\r\n");
		for(i=0;i<8;i++)
		{
			canbuf[i]=cnt+i;//��䷢�ͻ�����
			printf("%d ",canbuf[i]);
 		}
		printf("\r\n");
		
		res=CAN2_Send_Msg(canbuf,8);//����8���ֽ� 
		if(res)printf("����ʧ�ܦ\r\n");		//��ʾ����ʧ��
		else printf("���ͳɹ�\r\n");	 		//��ʾ���ͳɹ�	
		
		res=CAN2_Receive_Msg(canbuf);
		if(res)//���յ�������
		{			
 			printf("���յ�������Ϊ:\r\n");
			for(i=0;i<res;i++)
			{									    
				printf("%d ", canbuf[i]);
 			}
			printf("\r\n");
		}
		
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

//SPI2��������
void spi2_task(void *p_arg)
{
	OS_ERR err;
	
	while(1){
		
		//SPI1_ReadWriteByte(0xaa);	//SPI2���߶�д�����ֽ�
		SPI2_ReadWriteByte(0x55);
		
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


