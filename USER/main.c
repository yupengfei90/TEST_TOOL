#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "exit.h"
#include "sram.h"
#include "malloc.h"
#include "includes.h"
//ALIENTEK 探索者STM32F407开发板 UCOSIII实验
//例13-1 UCOSIII 同时等待多个内核对象

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

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

//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);  	//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
//	uart_init(1382400);  //串口初始化
	uart_init(2100000);
	LED_Init();         //LED初始化	
	exit0_init();
//	LCD_Init();			//LCD初始化	
	KEY_Init();			//按键初始化
//	FSMC_SRAM_Init();	//初始化SRAM
	my_mem_init(SRAMIN);//初始化内部RAM
	

	OSInit(&err);		    	//初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区			 
	//创建开始任务
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
	
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}


void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();
	//创建LED3任务
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
					
	
	//USART2接收处理任务
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
					
					
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSTaskDel((OS_TCB *)0, &err);
}

//LED3闪烁任务
void led3_task(void *p_arg)
{
	OS_ERR err;
	
	while(1)
	{
		LED3 = ~LED3;
		printf("1234567\n");
		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


//USART2接收处理任务
void usart2rec_task(void *p_arg)
{
	OS_ERR err;
	u16 t, len;
	
	while(1){
		if(USART_RX_STA & 0x8000){
			printf("\r\n接收到的数据为: \r\n");
			len = (USART_RX_STA & 0x3FF);
			for(t=0;t<len;t++){
				printf("%c",USART_RX_BUF[t]);
			}
			printf("\n");
			USART_RX_STA = 0;
		}		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


	


