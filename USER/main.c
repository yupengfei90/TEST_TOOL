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
#include "74hc595.h"
#include "DAC7565.h"
#include "AD7328.h"
#include "HCF4051.h"
#include "power_sw.h"
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


//协议部分
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

//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);  	//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	uart_init(115200);  //串口初始化
//	uart_init(2100000);
	LED_Init();         //LED初始化	
	exit0_init();
	KEY_Init();
//	LCD_Init();			//LCD初始化	
//	FSMC_SRAM_Init();	//初始化SRAM
	my_mem_init(SRAMIN);//初始化内部RAM
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);//CAN初始化环回模式,波特率500Kbps 	
	Init_74HC595();
	DAC7565_Init();
	AD7328_Init();
	SPI_Config();
	SPI2_SetSpeed(SPI_BaudRatePrescaler_16); //高速模式(42/4)M SPI
	power_sw_init();
	//AD7328_ChannelRead(0);
	//delay_ms(2000);
	HCF4051_Init();
	KL30_EN = 1;
	delay_ms(1000);
//	delay_ms(2000);
	

#if 0
	while(1){
		u16 ad;
		if(KEY_User){
//			AD7328_ChannelSelect(3);
//			HCF4051_ChannelSelect(0);
			
			AD7328_Sample(Power_Vol_AD);
//			delay_ms(10);
//			AD7328_ChannelSelect(3);
//			ad = AD7328_ReadAD();
//			printf("ad = 0x%X\r\n",ad);
			delay_ms(1000);
		}
	}
#endif
	
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
					
	//创建按键按下响应任务
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
					
	//创建SPI2测试任务
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

//		printf("1234567\n");
		
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
			//printf("\r\n接收到的数据为: \r\n");
			len = (USART_RX_STA & 0x3FF);
			for(t=0;t<len;t++){
				printf("%X ",USART_RX_BUF[t]);
			}
			printf("\r\n");
			
			if (USART_RX_BUF[0] == cmd_head[0] && USART_RX_BUF[1] == cmd_head[1] &&  \
				USART_RX_BUF[len-2] == cmd_tail[0] && USART_RX_BUF[len-1] == cmd_tail[1])
			{
				printf("正确的命令格式\r\n");
			}
			USART_RX_STA = 0;
		}		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


//用户按键按下响应任务处理任务
void key_task(void *p_arg)
{
	OS_ERR err;
	u8 i=0,res=0;
	u8 cnt=0;
	u8 canbuf[8];
	
	while(1){
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
		printf("\r\n发送的数据为:\r\n");
		for(i=0;i<8;i++)
		{
			canbuf[i]=cnt+i;//填充发送缓冲区
			printf("%d ",canbuf[i]);
 		}
		printf("\r\n");
		
		res=CAN2_Send_Msg(canbuf,8);//发送8个字节 
		if(res)printf("发送失败r\n");		//提示发送失败
		else printf("发送成功\r\n");	 		//提示发送成功	
		
		res=CAN2_Receive_Msg(canbuf);
		if(res)//接收到有数据
		{			
 			printf("接收到的数据为:\r\n");
			for(i=0;i<res;i++)
			{									    
				printf("%d ", canbuf[i]);
 			}
			printf("\r\n");
		}
		
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

//SPI2测试任务
void spi2_task(void *p_arg)
{
	OS_ERR err;
//	u8 i;
	while(1){
		
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
		//SPI1_ReadWriteByte(0xaa);	//SPI2总线读写两个字节
		//SPI2_ReadWriteByte(0x55);
//		for(i=0;i<4;i++)
//			DAC7565_Output(i,i+1);	
//		range_register_set();
		
//		SPI_ReadTest();
//		AD7328_ChannelRead(0);

		
		AD7328_Sample(Power_Vol_AD);

//		DAC7565_Output(0,3.6);
//		SPI2_ReadWriteByte(0x55);
		
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}


